import machine
Pin = machine.Pin

d0 = Pin(16)
flashbutton = d0
d1 = Pin(5)
d2 = Pin(4)
d3 = Pin(0)
d4 = Pin(2)
tx = Pin(1)
rx = Pin(3)
onboardled = d4
d5 = Pin(14)
d6 = Pin(12)
d7 = Pin(13)
d8 = Pin(15)
d9 = Pin(9)
sd2 = d9
sdd2 = d9
d10 = Pin(10)
sd3 = d10
sdd3 = d10
a0 = machine.ADC(0)

# mainly based on:
# https://samnicholls.net/wp-content/uploads/2016/11/esp8266_devkit_horizontal-01.png
# These are all internal for the esp8266 and modify the internal flash
# clk = Pin(6)
# sd0 = Pin(10)
# cmd = Pin(11)
# sd1 = Pin(8)