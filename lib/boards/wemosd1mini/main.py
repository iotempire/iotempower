# main.py script for ulnoiot wemosd1mini version
#
import gc
import machine
#machine.freq(160000000) # be fast by default

# convenient reset
class RESET:
    def __repr__(self):
        machine.reset()
        return ""
    def __call__(self):
        return self.__repr__()
reset = RESET()
reboot=reset

# convenient reset
class HELP:
    def __repr__(self):
        return """Welcome to the ulnoiot environment.

Try the following commands:
- wifi("name","pw") connects to wifi, wscan scans, wip shows address
- man to see file operation commands
- reset or reboot to reset the board
- help or help() shows this again

Also make sure to make use of tab-completion (just hit the tab  key
once or twice to see possible commands).

Go to http://iot.ulno.net for more information.
"""
    def __call__(self):
        print(self.__repr__())
        return ""

help = HELP()

from upysh import ls,rm,cd,clear,pwd,mkdir,head,cat,mv,rmdir,newfile,man

from ulnoiot import *
gc.collect()
print(clear())
print(help())

try:
    import user_boot
except ImportError:
    print("Create a file user_boot.py,\nif you want to start a project automatically.")
