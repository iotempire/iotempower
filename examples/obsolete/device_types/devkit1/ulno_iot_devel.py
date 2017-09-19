# ulno_iot developer testing shield
# also useful in eduction to get started

from machine import Pin

# configure leds
d4=Pin(2,Pin.OUT)
blue=d4 # TODO:reverse
blue.high() # switch off, too bright
d7=Pin(13,Pin.OUT)
red=d7
d8=Pin(15,Pin.OUT)
yellow=d8
# configure buttons
#	d0=Pin(16,Pin.IN,Pin.PULL_UP) needs hw pull-up (port 16 is special)
d0=Pin(16,Pin.IN)
lower_button=d0
d6=Pin(12,Pin.IN,Pin.PULL_UP)
right_button=d6
d3=Pin(0,Pin.IN,Pin.PULL_UP)
left_button=d3
#flash_button=d3
