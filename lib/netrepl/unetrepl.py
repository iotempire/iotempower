# This is based on https://github.com/cpopp/MicroTelnetServer
#
# Major modifications started by ulno (http://ulno.net) on 2017-09-15
# adding chacha encryption

_config_file = "/netrepl_config.py"
MAGIC = b"UlnoIOT-NetREPL:"

import time, socket, network, uos, machine, micropython, errno, ubinascii
from crypt_socket import Crypt_Socket

# debug
#from ulnoiot import *
#import ulnoiot.shield.devkit1_display
#dp=devices["dp1"]

_crypt_socket = None
_server_socket = None
_flush_timer = None
_telnetwrapper = None

# Provide necessary functions for dupterm and replace telnet control characters that come in.
class TelnetWrapper():
    BUFFER_SIZE=192
    INTERVAL=20 # reaction time in ms for buffering
    def __init__(self, crypt_socket):
        self.cs = crypt_socket
        self.in_buffer=None
        self.in_fill=0
        self.in_process=0
        self.out_buffer=bytearray(self.BUFFER_SIZE)
        self.out_fill=0
        self.out_last_sent=time.ticks_ms()
        self.out_buffer_lock = False
        
    def readinto(self, b):
        # TODO: check that this is non-blocking
        # return None, when no data available
        # else return the number of bytes read.
        if len(b) == 0:
            print("readinfo: empty buffer")
            return None # TODO:maybe we should then return 0?
        if self.in_process == self.in_fill: # more data needed
            (self.in_buffer,self.in_fill) = self.cs.receive()
            #print("r:",self.in_buffer[0:self.in_fill]) # debug
            self.in_process = 0
        if self.in_process < self.in_fill:
            r = self.in_buffer[self.in_process]
            if r==0x1e: # close requested
                self.close()
                return None
            else:
                b[0] = r
                #print("read",b[0])
                self.in_process+=1
                #if self.in_process < self.in_fill: return 1 # just handing over 1 byte
                return 1  # just handed over 1 byte
        return None # couldn't read anything

    def acquire_out_buffer(self):
        while self.out_buffer_lock == True:
            time.sleep_ms(1) # Wait for release
        self.irqstate=machine.disable_irq()
        if self.out_buffer_lock == True: # TODO: check if this locking is enough
            machine.enable_irq(self.irqstate)
            return False
        self.out_buffer_lock=True
        return True

    def release_out_buffer(self):
        self.out_buffer_lock=False
        machine.enable_irq(self.irqstate)

    def _send(self):
        # TODO: does this need to be unblocking?
        #dp.println("s {},{},{}".format(self.out_fill,int(self.out_buffer[0]),int(self.out_buffer[1]))) # debug
        self.cs.send(self.out_buffer,length=self.out_fill)
        self.out_fill = 0
        self.out_last_sent = time.ticks_ms()

    def _write1(self, byte):
        self.acquire_out_buffer()
        #if byte == 0 or byte == 0xff: return # TODO: debug
        #if byte != 10 and byte != 13 and (byte < 32 or byte > 127): return # TODO: debug
        #if byte==0 or byte>127: return # not sending this
        self.out_buffer[self.out_fill] = byte
#        dp.println("f1 {},{}".format(self.out_fill, self.MAXFILL))
        self.out_fill += 1
#        dp.println("f2 {},{}".format(self.out_fill, self.MAXFILL))
        if self.out_fill >= self.BUFFER_SIZE:
#            dp.println("f3 {},{}".format(self.out_fill,self.MAXFILL))
            self._send()
        self.release_out_buffer()

    def flush(self):
        t = time.ticks_ms()
        if self.out_fill == 0: # reset time, if there is nothing to send
            self.out_last_sent = t
            # debug dp.println("rt {}".format(t))
        elif time.ticks_diff(t, self.out_last_sent) > self.INTERVAL:
            # debug           dp.println("t {},{},{}".format(time.ticks_diff(t,self.out_last_sent),
            #                                           t,self.out_last_sent))
            self.acquire_out_buffer()
            self._send()
            self.release_out_buffer()

    def write(self, data):
        # sadly not called without input, makes buffering tricky
        # requires the scheduled flush
        for byte in data:
            self._write1(byte)
        self.flush()

    def close(self, report=False):
        # TODO: empty network buffers first?
        if report:
            self.write(b"\x1e")
        micropython.schedule(stop_client, 0)  # we are maybe in interrupt

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
    global _crypt_socket, netrepl_config, _telnetwrapper
    
    if _crypt_socket is not None:
        # close any previous clients
        uos.dupterm(None)
        _crypt_socket.close()

    client_socket, remote_addr = telnet_server.accept()
    print("\nnetrepl: Connection request from:", remote_addr[0])

    # prepare answer channel
    client_socket.setblocking(False)
    client_socket.setsockopt(socket.SOL_SOCKET, 20, uos.dupterm_notify)

    # read magic and initialization in first 2s
    readbytes = 0
    start_t = time.ticks_ms()
    block=bytearray(24)
    while readbytes < 24:
        try:
            received = client_socket.recv(1)
            if received and len(received) > 0:
                block[readbytes] = received[0]
                readbytes += 1
        except OSError as e:
            if len(e.args) > 0 \
                    and e.args[0] == errno.EAGAIN:
                pass  # try eventually again
            else:
                raise
        if time.ticks_diff(time.ticks_ms(), start_t) >= 2000:
            break

    _crypt_socket=Crypt_Socket(client_socket)

    if readbytes == 24 and block[0:16]==MAGIC:
        print("netrepl: Received initialization request and vector.")

        # setup input encryption
        _crypt_socket.init_in(netrepl_config.key, block[16:24])

        # read (now encrypted) magic word (16byte="UlnoIOT-NetREPL:"),
        # key (32byte=256bit) and iv (8byte=64bit)
        # but encrypted
        #init=decrypt_receive(last_client_socket,64,2000) # 2s timeout for init
        (init,l)=_crypt_socket.receive(request=56,timeoutms=2000) # 2s timeout for init
        if l==56 and init[0:16] == MAGIC: # Magic correct
            print("netrepl: Initial handshake succeeded, received session key.\n")
            # use rest for output key
            _crypt_socket.init_out(init[16:48],init[48:56])

            # print in terminal: last_client_socket.sendall(bytes([255, 252, 34])) # dont allow line mode
            # print in terminal: last_client_socket.sendall(bytes([255, 251, 1])) # turn off local echo

            _telnetwrapper = TelnetWrapper(_crypt_socket)
            uos.dupterm(_telnetwrapper)

            # construct timer to flush buffers often enough
            init_flush_timer()
            return

    # something went wrong
    client_socket.sendall('\nnetrepl: Wrong protocol for ulnoiot netrepl.\n')
    print("\nWrong protocol for this client. Closing.\n")
    _crypt_socket.close()
    _crypt_socket=None


def stop_client(t=None):  # allow random parameter for being scheduled
    global _crypt_socket, _flush_timer

    uos.dupterm(None)
    if _crypt_socket:
        _crypt_socket.close()
        _crypt_socket=None
        print("\nnetrepl: Connection closed.\n")
    if _flush_timer:
        _flush_timer.deinit()

def stop():
    global _server_socket
    stop_client()
    if _server_socket:
        _server_socket.close()



# start listening for telnet connections on port 23
def start(port=23,key=None,nostop=False): # TODO: take simpler default key as it will be reset
    global _server_socket, netrepl_config

    if nostop: # we want to check if it's already running and not restart it
        if _server_socket: # not none
            return # no new intialization _> stop here

    stop()

    if key is None:
        key=netrepl_config.key
    if key is None or len(key)==0:
        key=bytearray(32) # empty default key
    elif len(key) == 64:
        key=ubinascii.unhexlify(key)

    netrepl_config.key = key

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

connect=start # also access as connect

# write config and connect
def setup(key, reset=True):
    global netrepl_config

    if len(key) != 64:
        print("Key needs to be 64 bytes hex-code (256bit key).")
        return
    f = open(_config_file, "w")
    f.write("key=\"{}\"".format(key))
    f.close()
    print("Updated netrepl_config.")
    netrepl_config.key = key
    if reset:
        print("Resetting netrepl in 3 seconds.")
        time.sleep(3)
        start()
    else:
        print("Netrepl not restarted. Call start manually to restart.")


# Try to find and load netrepl_config
try:
    import netrepl_config
except ImportError:
    class netrepl_config():
        pass
    netrepl_config.key=None
#start(key=netrepl_config.key)
