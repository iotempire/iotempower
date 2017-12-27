class A32():
    def __init__(self, a):
        self.a = bytearray(a)

    @micropython.viper
    def __getitem__(self, k: int) -> uint:
        return uint(ptr32(self.a)[k])

    @micropython.viper
    def __setitem__(self, k: int, v: uint):
        ptr32(self.a)[k] = v

    def copy(self):
        return A32(self.a[:])

    def into(self, d):
        d.a[:] = self.a[:]

    def raw(self):
        return self.a
