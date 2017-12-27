# autostart.py is the autostart code for a ulnoiot node.
#
# this node is named 'buttons' and has three sensors:
#  - button1: sends messages "up" (not pressed) and "down" (button pressed)
#
# Sensor Wiring
#  Button 1
#   - S -> D4
#   - GND -> GND
#   - VCC -> +3V


from ulnoiot import *

button("button1", d4, "up", "down")

run(10)
