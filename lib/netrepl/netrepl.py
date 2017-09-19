# This is based on https://github.com/cpopp/MicroTelnetServer
#
# Major modifications started by ulno (http://ulno.net) on 2017-09-15
# adding chacha encryption

MAGIC = b"UlnoIOT-NetREPL:"

import socket
import network
import uos
import chacha
import time
import machine
import micropython

# debug
#from ulnoiot import *
#import ulnoiot.shield.devkit1_display
#dp=devices["dp1"]

_client_socket = None
_server_socket = None
_flush_timer = None
_telnetwrapper = None

# Provide necessary functions for dupterm and replace telnet control characters that come in.
class TelnetWrapper():
    MAXFILL=63 # best: multiples of 63
    INTERVAL=64 # 64 ms for buffering
    def __init__(self, cc_in, cc_out):
        self.cc_in = cc_in
        self.cc_out = cc_out
        self.discard_count = 0
        self.in_buffer=bytearray(self.MAXFILL)
        self.in_fill=0
        self.in_process=0
        self.out_buffer=bytearray(self.MAXFILL)
        self.out_fill=0
        self.out_last_sent=time.ticks_ms()
        self.sending = False
        
    def readinto(self, b):
        lb=len(b)
        if lb == 0: return None
        if self.in_process == self.in_fill:
            (data,lbuffer)=self.cc_in.receive()
            self.in_buffer[0:lbuffer]=data[0:lbuffer]
            #print("received:",self.in_buffer[0:lbuffer]) # debug
            self.in_fill = lbuffer
            self.in_process = 0
        if self.in_process < self.in_fill:
            b[0] = self.in_buffer[self.in_process]
            self.in_process+=1
            if self.in_process < self.in_fill: return 1 # still sth. left
        return None

    def _send(self):
        if self.sending == True: return # TODO: check if this locking is enough
        self.sending=True
# debug        dp.println("s {},{},{}".format(self.out_fill,int(self.out_buffer[0]),int(self.out_buffer[1])))
        self.cc_out.send(self.out_buffer,length=self.out_fill)
        self.out_fill = 0
        self.out_last_sent = time.ticks_ms()
        self.sending=False

    def _write1(self, byte):
        #if byte == 0 or byte == 0xff: return # TODO: debug
        #if byte != 10 and byte != 13 and (byte < 32 or byte > 127): return # TODO: debug
        self.out_buffer[self.out_fill] = byte
#        dp.println("f1 {},{}".format(self.out_fill, self.MAXFILL))
        self.out_fill += 1
#        dp.println("f2 {},{}".format(self.out_fill, self.MAXFILL))
        if self.out_fill >= self.MAXFILL:
#            dp.println("f3 {},{}".format(self.out_fill,self.MAXFILL))
            self._send()

    def flush(self):
        t = time.ticks_ms()
        if self.out_fill == 0: # reset time, if there is nothing to send
            self.out_last_sent = t
            # debug dp.println("rt {}".format(t))
        elif time.ticks_diff(t, self.out_last_sent) > self.INTERVAL:
            # debug           dp.println("t {},{},{}".format(time.ticks_diff(t,self.out_last_sent),
            #                                           t,self.out_last_sent))
            self._send()

    def write(self, data):
        # sadly not called without input, makes buffering tricky
        for byte in data:
            self._write1(byte)
        self.flush()

    def close(self):
        self.cc_in.socket.close()


def flush(t):
    # callback for timer to flush buffer (scheduled and safe to execute)
    global _telnetwrapper
    _telnetwrapper.flush()
    init_flush_timer()


def _flush_critical(t):
    # callback for timer to flush buffer (called in interrupt)
    micropython.schedule(flush,0)


def init_flush_timer():
    global _flush_timer
    if _flush_timer is None:
        _flush_timer=machine.Timer("netrepl")
    else:
        _flush_timer.deinit()
    _flush_timer.init(period=TelnetWrapper.INTERVAL,
                      mode=machine.Timer.ONE_SHOT,
                      callback=_flush_critical)


# Attach new clients to dupterm and 
# send telnet control characters to disable line mode
# and stop local echoing
def accept_telnet_connect(telnet_server):
    global _client_socket, _key, _telnetwrapper
    
    if _client_socket:
        # close any previous clients
        uos.dupterm(None)
        _client_socket.close()

    _client_socket, remote_addr = telnet_server.accept()
    print("\nnetrepl: Socket connection from:", remote_addr)

    # reset encryption status vector TODO: consider variable iv
    cc_in = chacha.ChaCha(_key, bytearray(8), socket=_client_socket)

    # prepare answer channel
    _client_socket.setblocking(False)
    _client_socket.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)

    # read magic word (16byte="UlnoIOT-NetREPL:"),
    # key (32byte=256bit) and iv (8byte=64bit)
    # but encrypted
    #init=decrypt_receive(last_client_socket,64,2000) # 2s timeout for init
    (init,l)=cc_in.receive(timeoutms=2000) # 2s timeout for init
    if l==56 and init[0:16] == MAGIC: # Magic correct
        print("\nnetrepl: Initial handshake succeeded, received session key.")
        # use rest for output key
        cc_out =  chacha.ChaCha(init[16:48],init[48:56], socket=_client_socket)

        # print in terminal: last_client_socket.sendall(bytes([255, 252, 34])) # dont allow line mode
        # print in terminal: last_client_socket.sendall(bytes([255, 251, 1])) # turn off local echo

        _telnetwrapper = TelnetWrapper(cc_in,cc_out)
        uos.dupterm(_telnetwrapper)
        # while True:
        #     d=cc_in.receive()
        #     if len(d)>0:
        #         d=bytes(d).decode().strip()
        #         print(d)
        #         if d.startswith("quit"):
        #             print("done")
        #             break
        #         v=None
        #         try:
        #             v=eval(d)
        #             print("result", v)
        #             cc_out.send(str(v))
        #         except Exception as e:
        #             print(e.args[0])
        #             cc_out.send(str(v))

        init_flush_timer()
        # construct timer to flush buffers often enough

    else:
        _client_socket.sendall('\nnetrepl: Wrong protocol for ulnoiot netrepl.\n')
        _client_socket.close()
        print("\nWrong protocol for this client. Closing.\n")


def stop():
    global _server_socket, _client_socket
    uos.dupterm(None)
    if _server_socket:
        _server_socket.close()
    if _client_socket:
        _client_socket.close()
    if _flush_timer:
        _flush_timer.deinit()


# start listening for telnet connections on port 23
def start(port=23,key=b'ulnoiot.netrepl.ulnoiot.netrepl.'): # TODO: take simpler default key as it will be reset
    stop()
    global _server_socket, _key
    _key = key

    # will be initialized after connection
    # cc_out = chacha.ChaCha(key, bytearray(8))
    _server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    ai = socket.getaddrinfo("0.0.0.0", port)
    addr = ai[0][4]
    
    _server_socket.bind(addr)
    _server_socket.listen(1)
    _server_socket.setsockopt(socket.SOL_SOCKET, 20, accept_telnet_connect)
    
    for i in (network.AP_IF, network.STA_IF):
        wlan = network.WLAN(i)
        if wlan.active():
            print("\nnetrepl: UlnoIOT netrepl server started on {}:{}".format(wlan.ifconfig()[0], port))
