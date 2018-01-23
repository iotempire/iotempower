import machine
Pin = machine.Pin

# left (antenna front and up, usb back down)
# RST
a0 = machine.ADC(0)
d0 = Pin(16)
d5 = Pin(14)
clk = d5
d6 = Pin(12)
miso = d6
d7 = Pin(13)
mosi = d7
d8 = Pin(15)
cs = d8
# 3.3V

# right
tx = Pin(1)
rx = Pin(3)
d1 = Pin(5)
d2 = Pin(4)
d3 = Pin(0)
d4 = Pin(2)
onboardled = d4
# GND
# 5V
