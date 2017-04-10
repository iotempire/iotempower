# main.py script for ulnoiot wemosd1mini version
#
import gc
import machine
#machine.freq(160000000) # be fast by default

from ulnoiot import *
gc.collect()

print(clear())
print(open("/ulnoiot/help/_general.txt").read())
print()

try:
    import user
except ImportError:
    print("Create a file user.py,\nif you want to start a project automatically.\n")
