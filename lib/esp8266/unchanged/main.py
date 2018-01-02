# main.py script for ulnoiot wemosd1mini version
#
import gc
from ulnoiot import *

gc.collect()

# Try to setup network/gw connection (i.e. mqtt)
try:
    import gw_cfg
except ImportError:
    pass
gc.collect()

# try to autostart user.py
try:
    import autostart
except ImportError:
    print(clear())
    help()
    help("autostart.py")
