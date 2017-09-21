# Network access layer for netrepl
#
# Author ulno (http://ulno.net)
# Created: 2017-09-20

import time
import errno
from chacha import ChaCha

sockwrite=False

try: # micropython
    from micropython import const
    from time import ticks_ms, ticks_diff,sleep_ms
    sockwrite=True
except: # normal python
    def const(a): return a
    def ticks_ms(): return int(time.clock() * 1000)
    def ticks_diff(a, b): return a - b
    def sleep_ms(t): time.sleep(t/1000)

NETBUF_SIZE = const(192)

class Crypt_Socket:

    def __init__(self, socket):
        self.netbuf = bytearray(NETBUF_SIZE)
        self.crypt_in = None
        self.crypt_out = None
        self.socket = socket
        socket.setblocking(False) # non blocking

    def init_in(self, key, iv):
        self.crypt_in = ChaCha(key,iv)

    def init_out(self, key, iv):
        self.crypt_out = ChaCha(key,iv)

    def send(self, datain, length=None):
        #print("Sending",datain)
        if length is None: l=len(datain)
        else: l=length
        if l>NETBUF_SIZE:
            raise Exception("Can't send packages longer than {} bytes.".format(NETBUF_SIZE))
        self.netbuf[0:l] = datain[0:l]
        self.crypt_out.encrypt(self.netbuf, length=l)
        self._write(l) # as padded, send full block

    def _write(self, length):
        global sockwrite
        # we need to write all the data but it's a non-blocking socket
        # so loop until it's all written eating EAGAIN exceptions
        data=memoryview(self.netbuf)
        written=0
        while written < length:
            try:
                if sockwrite:
                    written += self.socket.write(data[written:length])
                else:
                    written += self.socket.send(data[written:length])
            except OSError as e:
                if len(e.args) > 0 and e.args[0] == errno.EAGAIN:
                    # can't write yet, try again
                    pass
                else:
                    # something else...propagate the exception
                    raise


    def receive(self, request=0, timeoutms=None):
        # receive into network buffer,
        # fill buffer once and decrypt
        # if request>0 wait blocking for request number of bytes (if timeout
        # given interrupt after timeoutms ms)
        data = self.netbuf
        readbytes = 0
        start_t = ticks_ms()
        while readbytes < NETBUF_SIZE:
            try:
                received = self.socket.recv(1)
                if len(received) == 0: # no new data
                    if readbytes >= request:
                        break  # break if not blocking to request size
                else:
                    data[readbytes] = received[0]
                    readbytes += 1
            except OSError as e:
                if len(e.args) > 0 \
                        and (e.args[0] == errno.EAGAIN
                        or e.args[0] == errno.ETIMEDOUT):
                    if readbytes >= request:
                        break  # break if not blocking to request size
                else:
                    raise
            if timeoutms is not None \
                    and ticks_diff(ticks_ms(), start_t) >= timeoutms:
                break
            sleep_ms(1) # prevent busy waiting
        if readbytes>0: self.crypt_in.decrypt(data,length=readbytes)
        return data,readbytes

    def close(self):
        self.socket.close()
