from integriot import *

led1Status = False

init("localhost")  # use the MQTT broker on localhost

prefix("leds")  # all actors below are prefixed with /leds

led1 = actor("led1")


def button1Callback(msg):
    global led1Status

    print("received: [button]", msg)

    if (msg == "down"):
        led1Status = not led1Status  # toggle status of led

        if (led1Status):
            led1.publish("set", "on")  # publish updated state
            print("sending: [led1]", "on")
        else:
            led1.publish("set", "off")
            print("sending: [led1]", "of")


prefix("buttons")  # all sensors below are prefixed with /buttons

button1 = sensor("button1")
button1.subscribe_change(callback=button1Callback)

run()
