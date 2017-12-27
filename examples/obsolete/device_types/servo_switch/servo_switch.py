from machine import Pin, PWM
import servo
import time

# the default position will be
pos_on = 0
pos_mid = 90
pos_off = 179
s = servo.Servo(Pin(0))


def turn(angle):
    global s
    s.init()
    print("Turning to", angle)
    s.write_angle(angle)
    time.sleep(0.7)  # let it turn
    s.release()


def on():
    turn(pos_on)
    turn(pos_mid)


def off():
    turn(pos_off)
    turn(pos_mid)
