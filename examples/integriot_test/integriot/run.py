from integriot import *


def show(m):
    global s
    print("received:",m)
    if m == "off":
        s.toggle()


init("localhost")

prefix("testnode1")
sensor("b1",{"":(True,show)})
s = led("blue")

s.publish("set","off")

run()
