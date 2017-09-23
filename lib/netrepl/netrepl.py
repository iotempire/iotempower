#!/usr/bin/env python3
#
# netrepl connect library
# this is a connection class to be used in other tools
#
# author: ulno
# create date: 2017-09-21
#
# based on telnet example at http://www.binarytides.com/code-telnet-client-sockets-python/

# Magic word for the protocol start
MAGIC = b"UlnoIOT-NetREPL:"

import socket, sys, os, time
from crypt_socket import Crypt_Socket


def ticks_ms(): return int(time.time() * 1000)
def ticks_diff(a, b): return a - b
def sleep_ms(t): time.sleep(t / 1000)


class Netrepl:
    def __init__(self,host,port=23,key=None,debug=None):
        """
        Build a connection object for netrepl

        :param host: hostname or IP address
        :param port: port is by default 23 (like telnet)
        :param key: if key is None or "", an empty password (like in the
        initial configuration of a freshly installed microcontroller) is used.
        The key can also be a 32bit-byte string or array or a 64-byte hex
        string.
        """
        if key is None or len(key)==0:
            key=bytearray(32)
        elif len(key)==64:
            key=bytes.fromhex(key)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket=s
        s.settimeout(2)  # initially this is a blocking port (for 2s)

        # connect to remote host
        try:
            s.connect((host, port))
        except:
            if debug is not None: print(debug,"Unable to connect.")
            raise

        if debug is not None:
            print(debug, "Connected to remote host")
            print(debug, "Sending plain text initialization and initialization vector.")

        iv_out = os.urandom(8)
        s.send(MAGIC+iv_out)
        if debug is not None: print(debug,'Generating session key.')
        s.settimeout(0)  # now, we can be non blocking

        self.cs = Crypt_Socket(s)
        self.cs.init_out(key,iv_out)
        key_in = os.urandom(32)
        iv_in = os.urandom(8)
        if debug is not None:
            print(debug,'Sending initialization and session key.')

        self.cs.send(MAGIC+key_in+iv_in)  # send the key

        self.cs.init_in(key_in, iv_in)

        if debug is not None:
            print(debug, 'Waiting for connection.')
        time.sleep(0.5)
        self.debug = debug

    def send(self,data):
        """
        Send all data given out (and block until all is sent).
        :param data:
        :return:
        """
        self.cs.send(bytearray(data))

    def receive(self,request=0,timeoutms=0):
        """
        Try to none blockingly read data. (Block if request>0 until received or
        timeout happened - try to read at least request bytes.)

        :param data:
        :param request: if > 0 wait for actual data to arrive
        :param timeoutms:
               timeoutms == None: block infinitely
               timeout==0: return immediately after trying to read something from network buffer
               timeout>0: try for time specified to read something before returning
        :return: a newly allocated byte-buffer with the received data or None,
        if nothing received
        """
        (data,l)=self.cs.receive(request,timeoutms)
        if l==0:
            return None
        return bytes(data[0:l])

    def repl_raw(self):
        self.cs.send(b"\x01")

    def repl_normal(self):
        self.cs.send(b"\x02")

    def repl_interrupt(self):
        self.cs.send(b"\r\n\x03\x03\r\n")  # Interrupt
        time.sleep(0.5)  # wait for interrupt to finish

    def repl_execute(self):
        self.cs.send(b"\x04")

    def repl_close(self):
        self.cs.send(b"\x1e")  # ctrl-]

    def read_until(self,term,timeoutms=None):
        """
        Read until given term-string is found, return all data read until then

        :param term:
        :param timeoutms:
        :return: data read or None if interrupted by timeout
        """
        buffer = bytearray(0)
        starttime = ticks_ms()
        while True:
            next=self.receive(request=1,timeoutms=0)
            if next is not None:
                buffer.append(next[0])
                #if(len(buffer)%40==0): print(len(buffer),buffer[-40:]) # debug
            if buffer.endswith(term):
                return buffer[0:-len(term)]
            if timeoutms is not None:
                if ticks_diff(ticks_ms(),starttime) >= timeoutms:
                    return None
            sleep_ms(10) # give some time for filling buffer
        # should not come here

    def repl_command(self, command, timeoutms=None, interrupt=True):
        """
        Execute a command remotely and return output.
        :param command:
        :param timeoutms: None (block), >=0 block until given time.
        :param interrupt: By default try to interrupt current process.
        :return:
        """
        if self.debug: print(self.debug, 'Sending command.')
        if type(command) is str:
            command=command.encode()
        if interrupt:
            self.repl_interrupt()
        self.repl_raw()
        self.read_until(b"raw REPL; CTRL-B to exit\r\n>", timeoutms=2000)
        self.send(command)
        self.repl_execute()
        self.read_until(b"OK", timeoutms=2000)  # wait for output start
        return self.read_until(b"\x04\x04>", timeoutms=timeoutms)


    def close(self,report=False):
        if report:
            self.repl_close()
        self.cs.close()


class Netrepl_Parser():
    def __init__(self,description,usage="%(prog)s [key|port [key]]",debug="netrepl:"):
        import argparse # only import when used
        parser = argparse.ArgumentParser(usage=usage,
                                         description=description)
        parser.add_argument('host', type=str,
                            help='Hostname or ip to connect to.')
        parser.add_argument('port', type=str, nargs='?',
                            help='Port (Default=23)')
        parser.add_argument('key', type=str, nargs='?',
                            help='Key as 32-byte string or 64-byte-hex code. '
                                 'If "", then create empty default key for initial '
                                 'connect.'
                                 'Key needs to be 256bit in either bytes or hex notation, '
                                 'If key is not given as option it is read as newline '
                                 'terminated 64 byte hex notation from stdin')
        self.parser=parser
        if debug is not None:
            if debug.endswith(":"):
                self.debug = debug
            else:
                self.debug = debug+":"
        else:
            self.debug = None
        self.parsed = False

    def parse(self):
        args = self.parser.parse_args()
        if args.port is not None:
            if args.port.isdigit():
                args.port = int(args.port)
            else:
                args.key = args.port
                args.port = 23
        else:
            args.port = 23

        key = args.key
        if key is None:
            print('Enter key (32bytes as hex->64bytes):')
            key = sys.stdin.readline().strip().encode()
            if len(key) != 64 and len(key) != 0:
                print("Key has to specified as 64 byte hex. Exiting.")
                sys.exit(1)
        if len(key) == 64:
            key = bytes.fromhex(key)
        if len(key) == 0:
            key = bytearray(32)
        if len(key) != 32:
            print("Key doesn't resolve to 256bit (32 bytes). Exiting.")
            sys.exit(1)

        self.host = args.host
        self.port = args.port
        self.key = key
        self.args = args

    def connect(self):
        if not self.parsed:
            self.parse()
        con = Netrepl(self.host, self.port, self.key, debug=self.debug)
        return con


