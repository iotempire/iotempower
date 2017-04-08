from ulnoiot import *

# general init
w=wifi( "ehdemo-iotempire","internetofthings" )
mqtt( "192.168.10.254","testfloor/devkit1-01",user="homeassistant",password="internetofthings",client_id="devkit1-01")

# devkit1 button/led shield
button("lower",d0)
button("left",d3)
button("right",d6)
out("blue",onboardled,"off","on")
out("red",d7)
out("yellow",d8)

# devkit1 lcd screen
d=display("text",d2,d5)
d.clear()
d.println("iot.ulno.net")
d.println()
d.println(w.ipaddress)

# devkit1 humidity temperature sensor
ht(d1)

# transmit loop, transmit all values every 5ms
transmit(5)

