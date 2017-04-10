import gc
import machine
from machine import ADC, PWM

Pin=machine.Pin

d0=Pin(16)
d1=Pin( 5)
d2=Pin( 4)
d3=Pin( 0)
d4=Pin( 2)
onboardled=d4
d5=Pin(14)
d6=Pin(12)
d7=Pin(13)
d8=Pin(15)
a0=machine.ADC(0)

from . import _wifi
wifi=_wifi.setup
wscan = _wifi.scan
wip = _wifi.wip

# convenient reset
def reset():
    machine.reset()
reboot=reset

from upysh import ls,rm,cd,clear,pwd,mkdir,head,cat,mv,rmdir,newfile
# overwrite man

gc.collect()


from ulnoiot.devices import *

# build help-dictionary file for all visible things
_excludes=["_excludes", "_helplist", "i",
           "__name__", "__path__",
           "HELP", "WIP",
           "gc", "machine"]
_helplist = {}
for i in dir():
    if i not in _excludes:
        t=eval(i)
        if t is not None and \
                type(t) != type(machine) and \
                not isinstance(t, Pin) and \
                not isinstance(t, PWM) and \
                not isinstance(t, ADC): # not a module or weird classes
            _helplist[t]=i

from ulnoiot.help import help, man

gc.collect()
