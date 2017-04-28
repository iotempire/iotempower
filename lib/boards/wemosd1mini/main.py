# main.py script for ulnoiot wemosd1mini version
#
import gc
import machine
#machine.freq(160000000) # be fast by default

from ulnoiot import *
gc.collect()

print(clear())
help()

try:
    import user
except ImportError:
    help("user.py")
