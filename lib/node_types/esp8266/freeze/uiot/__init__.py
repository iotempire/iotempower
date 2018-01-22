# Init ulnoiot and
# get all names into namespace

import gc
import machine


from pins import *  # get all the pins

from . import _wifi

wifi = _wifi.setup
wifi_connect = _wifi.connect
wscan = _wifi.scan
wip = _wifi.wip


# convenient reset
def reset():
    machine.reset()


reboot = reset

# shell stuff
from uiot.sh import ls, rm, cd, clear, pwd, mkdir, head, cat, mv, rmdir, newfile, cp, copy

gc.collect()
from uiot._mgr import *
from uiot._mgr import _devlist as devices

gc.collect()
import uiot._help

help = uiot._help.help
man = uiot._help.man
gc.collect()
