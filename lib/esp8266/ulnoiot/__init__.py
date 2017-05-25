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
wifi_connect=_wifi.connect
wscan=_wifi.scan
wip=_wifi.wip

# convenient reset
def reset():
    machine.reset()
reboot=reset

from ulnoiot.ulnoupysh import ls,rm,cd,clear,pwd,mkdir,head,cat,mv,rmdir,newfile,cp,copy

gc.collect()
from ulnoiot.devices import *
from ulnoiot.devices import _devlist as devices
gc.collect()
import ulnoiot.help as _help
help=_help.help
man=_help.man
gc.collect()
