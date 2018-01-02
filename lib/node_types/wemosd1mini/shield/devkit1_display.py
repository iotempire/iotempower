# Shield from ulno's iot devkit 1 with display
#

from ulnoiot import *

# devkit1 lcd screen
dp = display("dp1", d2, d5)
dp.println("Current IP: ")
dp.println(wip)
