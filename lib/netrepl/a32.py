# workaround for addressing bytearrays like in c with pointer casting
# this is accelerated on micropython

import struct

_packstr="<1I" # unsigned 32-bit integer

class A32():
    def __init__(self,a):
        self.a=bytearray(a)
    def __getitem__(self,k):
        return struct.unpack_from(_packstr,self.a,k*4)[0]
    def __setitem__(self,k,v):
        struct.pack_into(_packstr,self.a,k*4,v)
    def copy(self):
        return A32(self.a[:])
    def into(self,d):
        d.a[:]=self.a[:]
    def raw(self):
        return self.a

