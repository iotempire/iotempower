# autostart.py is the autostart code for a ulnoiot node.
#
# this node is named 'infoboardleds and has one actors:
#  - led1: the onboard led, can be switched on with the "on" message and off with the "off" message
#
# Actor Wiring
#  led1: the onboard led is used


from ulnoiot import *


led("led1", onboardled, "off", "on")


run(10)
