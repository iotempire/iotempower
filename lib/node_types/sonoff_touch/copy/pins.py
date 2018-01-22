import machine
Pin = machine.Pin

d0 = Pin(16)
d1 = Pin(5)  # d1 relais 2
relais2 = d1
d2 = Pin(4)
d3 = Pin(0)  # d3 touchbutton 1
touchbutton1 = d3
d4 = Pin(2)  # d4 touchbutton 2
touchbutton2 = d4
tx = Pin(1)
rx = Pin(3)
onboardled = d4
d5 = Pin(14)
d6 = Pin(12)  # d6 relais 1
relais1 = d6
d7 = Pin(13)  # d7 led

d8 = Pin(15)
a0 = machine.ADC(0)
