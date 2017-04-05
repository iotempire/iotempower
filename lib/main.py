# main.py script for ulno's version of wemos shields
#

import machine
#machine.freq(160000000) # be fast by default

# convenient reset
import machine
def reset():
	machine.reset()

import wifi

from upysh import *

import user_boot
