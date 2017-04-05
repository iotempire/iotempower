# main.py script for ulno's version of wemos shields
#

import machine
machine.freq(160000000) # be fast by default

# convenient reset
import machine
def reset():
	machine.reset()

# the next sets up wifi or shows some boot instructions
# on a potentially attached screen
# also resets (clears wifi_config when lower button
# is pressed when starting up
import ulno_iot_boot
#from ulno_iot_boot import display
#if ulno_shield.present:
#	from ulno_shield01 import d3,d4,d5,d6,d7,left_button,right_button,lower_button,red,yellow,blue,display
# if wifi configured and present, start user specific boot
if ulno_iot_boot.wifi_configured:
	import user_boot
