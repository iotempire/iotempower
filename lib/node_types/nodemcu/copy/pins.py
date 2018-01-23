import machine
Pin = machine.Pin

# left (antenna is up, usb port down)
a0 = machine.ADC(0)
# reserved
# reserved
d10 = Pin(10)
sd3 = d10
sdd3 = d10
d9 = Pin(9)
sd2 = d9
sdd2 = d9
# based on:
# https://samnicholls.net/wp-content/uploads/2016/11/esp8266_devkit_horizontal-01.png
# https://alexbloggt.com/wp-content/uploads/2015/10/nodemcu_pinout_700.png
# These are all internal for the esp8266 and modify the internal flash
# sdd1 = Pin(8)
# sdcmd = Pin(11)
# sdd0 = Pin(10)
# sdclk = Pin(6)
# gnd
# 3.3V
# EN
# RST
# GND
# Vin

# right
d0 = Pin(16)
wake = d0
d1 = Pin(5)
d2 = Pin(4)
d3 = Pin(0)
flashbutton = d3
d4 = Pin(2)
onboardled = d4
# 3.3V
# GND
d5 = Pin(14)
clk = d5
d6 = Pin(12)
miso = d6
d7 = Pin(13)
mosi = d7
d8 = Pin(15)
cs = d8
rx = Pin(3)
tx = Pin(1)
# GND
# 3.3V
