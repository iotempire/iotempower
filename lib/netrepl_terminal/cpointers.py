# workaround for addressing bytearrays like in c with pointer casting
# this is accelerated on micropython

import struct

_packstr="<1I" # unsigned 32-bit integer

class A32():
    def __init__(self,a):
        self.a=bytearray(a)
    def __getitem__(self,k):
        k*=4
        return struct.unpack(_packstr,self.a[k:k+4])[0]
    def __setitem__(self,k,v):
        p=struct.pack(_packstr,v)
        k*=4
        self.a[k]=p[0]
        self.a[k+1]=p[1]
        self.a[k+2]=p[2]
        self.a[k+3]=p[3]
    def copy(self):
        return A32(self.a[:])
    def into(self,d):
        d.a[:]=self.a[:]
