import machine
Pin = machine.Pin

## left
# 3.3V
# EN
io14 = Pin(14)
clk = io14
io12 = Pin(12)
miso = io12
io13 = Pin(13)
mosi = io13
io15 = Pin(15)
cs = io15
io2 = Pin(2)
onboardled = io2
io0 = Pin(0)
flashbutton = io0
# gnd
# gnd

## right
io4 = Pin(4)
sda = io4
rx = Pin(3)
tx = Pin(1)
io5 = Pin(5)
scl = io5
# rst
a0 = machine.ADC(0)
io16 = Pin(16)
# gnd
# 5-12V
# 5-12V

# 3 buttons in upper right half
button_up = io0
button_mid = io13
# button_down = reset

# these are not broken out:
#d9 = Pin(9)
#sd2 = d9
#sdd2 = d9
#d10 = Pin(10)
#sd3 = d10
#sdd3 = d10
