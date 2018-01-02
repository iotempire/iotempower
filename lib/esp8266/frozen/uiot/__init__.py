import gc
import machine


# Get all names into namespace
from machine import ADC, PWM

from pin_cfg import *  # get all the pins

from . import _wifi

wifi = _wifi.setup
wifi_connect = _wifi.connect
wscan = _wifi.scan
wip = _wifi.wip


# convenient reset
def reset():
    machine.reset()


reboot = reset

from uiot.sh import ls, rm, cd, clear, pwd, mkdir, head, cat, mv, rmdir, newfile, cp, copy

gc.collect()
from uiot._mgr import *
from uiot._mgr import _devlist as devices

gc.collect()
import help as _help

help = _help.help
man = _help.man
gc.collect()
