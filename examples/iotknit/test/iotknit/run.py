from iotknit import *


def show(msg):
    global onboard_blue
    print("received:", msg)
    if msg == "off":
        onboard_blue.toggle()



init("localhost")

prefix("testnode1")
button = subscriber("b1")
button.subscribe_change(callback=show)
onboard_blue = switch_publisher("blue", init_state="off")

run()
