# TODO: verify for which sonoffs this exactly works
import machine
Pin = machine.Pin

d0 = Pin(16)

d1 = Pin(5)
relais2 = d1
redled2 = d1

d2 = Pin(4)
relais3 = d2
redled3 = d1

d3 = Pin(0)
button1 = d3
flashbutton = d3

d4 = Pin(2)
user2 = d4

tx = Pin(1)

rx = Pin(3)

d5 = Pin(14)
user1 = d5
button4 = d5

d6 = Pin(12)
relais1 = d6
redled1 = d6

d7 = Pin(13)
onboardled = d7
greenled = d7
blueled = d7

d8 = Pin(15)
relais4 = d8
redled4 = d8

d9 = Pin(9)
button2 = d9

d10 = Pin(10)
button3 = d10
