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
NETBUF_SIZE=1024

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
        self.oldbuffer = bytearray(0)
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

        self.cs = Crypt_Socket(s,netbuf_size=NETBUF_SIZE)
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
        self.cs.send(b"\r\n\x03")  # Interrupt
        time.sleep(0.1)  # wait for interrupt to finish
        self.cs.send(b"\x03\r\n")  # Interrupt

    def repl_execute(self):
        self.cs.send(b"\x04")

    def repl_close(self):
        self.cs.send(b"\x1e")  # ctrl-]

    def read_until(self,term,timeoutms=None,fresh_buffer=False):
        """
        Read until given term-string is found, return all data read until then

        :param term:
        :param timeoutms:
        :return: data read or None if interrupted by timeout
        """
        if fresh_buffer:
            buffer = bytearray(0)
        else:
            buffer = self.oldbuffer
        buffer_p = 0
        term_len = len(term)
        starttime = ticks_ms()

        while True:
            # check if it's in the updated (or old) buffer
            newpos = buffer.find(term, buffer_p)
            # print("read_until:", term, newpos, buffer) # debug
            if newpos >= 0:
                self.oldbuffer = buffer[
                                 newpos + term_len:]  # keep rest for later
                return buffer[0:newpos]
            buffer_p = max(0, len(buffer) - term_len + 1)  # update searched space

            # if not in there, try to get some more data
            next = self.receive(request=NETBUF_SIZE,timeoutms=100)
            if next is not None:
                buffer.extend(next)

            # stop if timeout passed
            if timeoutms is not None:
                if ticks_diff(ticks_ms(),starttime) >= timeoutms:
                    return None
            sleep_ms(10) # give some time for filling buffer
        # should not come here

    def repl_command(self, command, timeoutms=5000, interrupt=False):
        """
        Execute a command remotely and return output.
        :param command:
        :param timeoutms: None (block), >=0 block until given time.
        :param interrupt: By default try to interrupt current process.
        :return:
        """
        if self.debug: print(self.debug, 'Sending command: >>{}<<'
                                            .format(command))
        if type(command) is str:
            command=command.encode()
        if interrupt:
            self.repl_interrupt()
        self.repl_raw()
        a=self.read_until(b"raw REPL; CTRL-B to exit\r\n>", timeoutms=timeoutms)
        if a is None:
            if self.debug: print(self.debug, 'Timeout waiting for raw REPL, '
                                             'aborting')
            return None
        self.send(command)
        self.repl_execute()
        a = self.read_until(b"OK", timeoutms=timeoutms)  # wait for output start
        if a is None:
            if self.debug: print(self.debug, 'Timeout waiting for OK, aborting')
            return None
        #print("oldbuffer",self.oldbuffer) # debug
        a=self.read_until(b"\x04\x04>", timeoutms=timeoutms)
        if a is None:
            if self.debug: print(self.debug, 'Timeout waiting for command '
                                             'execution, aborting.')
        if self.debug: print(self.debug, 'Returning answer: >>{}<<.'
                                            .format(a))
        return a

    def _guard(self, check, command):
        a = self.repl_command(command + '\r\nprint("{}")'.format(check),
                              timeoutms=5000)
        if type(check) is str: check = check.encode()
        if not a or not a.startswith(check):
            if self.debug: print(self.debug, "Copy communication "
                                             "failed for status {}."
                                 .format(check.decode()))
            return True  # this means failure
        return False  # success

    def upload(self,local,remote,remove=True):
        # print("upload",local,remote) # debug
        c = self._guard # shortcut

        remote_tmp=remote+".tmp_netrepl"
        if c('tmp_rm', 'import os\ntry: os.remove("{}")\nexcept: pass\n'
                .format(remote_tmp)): return # try to remove tmp
        f=open(local,"rb")
        if c('setup','import gc;gc.collect();f=open("{}","wb")'
                .format(remote_tmp)): return
        bnr=0
        while True:
            block = f.read(128)
            if len(block)==0: break
            if c('w%d'%bnr,'f.write({})'.format(block)): return
            bnr+=1
        f.close()
        if c('close','f.close()'): return
        if remove:
            if c('remove','import os;os.remove("{}")'.format(remote)): return
        if c('rename','import os;os.rename("{}","{}")'
                .format(remote_tmp,remote)): return

    def mkdir(self,path,nofail=False):
        """
        Create a directory on remote.
        :param nofail: if nofail is set, command does nto fail if directory
        already exists
        :return:
        """
        c = self._guard # shortcut

        if path.endswith("/"):
            path=path[:-1]
        command='mkdir("{}")'.format(path)
        if nofail:
            command="\r\ntry:\r\n {}\r\nexcept:\r\n pass\r\n".format(command)
        if c('mkdir','import os\r\n{}'.format(command)): return

    def reset(self):
        """
        Reset/reboot the board
        :return:
        """
        self.repl_command('import machine;machine.reset()')

    def rm(self,path):
        c = self._guard # shortcut

        if c("rm",'import os;os.remove("{}")'.format(path)): return

    def rmdir(self,path,recursive=False):
        c = self._guard # shortcut
        # TODO: add recursive delete

        if path.endswith("/"): # dir
            path=path[:-1]
        if c("rmdir", 'import os;os.remove("{}")'.format(path)): return

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
        parser.add_argument('--keyfile','--key-file', type=str, nargs='?',
                            help='Read key from specified file (last line in '
                                 'that file should be 64 bytes hex-key).')

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
            # Check if key was provided as keyfile
            if args.keyfile is not None:
                f=open(args.keyfile,"rb")
                for l in f:
                    s=l.strip()
                    if len(s) == 64:
                        key=l
                key=key.decode()
                f.close()
            else:
                print('Enter key (32bytes as hex->64bytes):')
                key = sys.stdin.readline().strip()
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


