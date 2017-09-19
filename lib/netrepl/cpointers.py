class p16():
    def __init__(self,a):
        self.a=a
    @micropython.viper
    def __getitem__(self,k:int)->uint:
        return uint(ptr16(self.a)[k])
    @micropython.viper
    def __setitem__(self,k:int,v:uint):
        ptr16(self.a)[k]=v
class p32():
    def __init__(self,a):
        self.a=a
    @micropython.viper
    def __getitem__(self,k:int)->uint:
        return uint(ptr32(self.a)[k])
    @micropython.viper
    def __setitem__(self,k:int,v:uint):
        ptr32(self.a)[k]=v
