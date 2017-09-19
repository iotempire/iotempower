from ulnoiot import *
from ulnoiot.shield.onboardled import blue
import time

wsensor=None

def test_turn_led(myid):
    global wsensor

    if myid=="blue_off":
        myid="blue_off2"
    else:
        myid="blue_off"
    if wsensor.value()==1:
        do_later(2,test_turn_led,id=myid) # check again in 2s and keep led on
    else:
        blue.high()


def light_up_blue(t):
    if t.value() == 1:
        blue.low()
        do_later(2,test_turn_led,id="blue_off")


def main():
    global wsensor

    blue.low() # turn on to signal start

    wsensor=analog("water",threshold=450, precision=10, on_change=light_up_blue)

    time.sleep(0.5)

    blue.high() # turn off

    run(60)

main()
