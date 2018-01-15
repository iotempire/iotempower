# this allows legacy ulnoiot-code (pre 2018-01) to still work
from uiot import *

def contact(name, *args, **kwargs):
    return d("contact", "Contact", name, *args, **kwargs)


button = contact
input = contact


def analog(name, *args, **kwargs):
    return d("analog", name, *args, **kwargs)


def trigger(name, *args, **kwargs):
    return d("trigger", name, *args, **kwargs)


def output(name, *args, **kwargs):
    return d("output", name, *args, **kwargs)


led = output
switch = output
out = output


def servo(name, *args, **kwargs):
    return d("servo", name, *args, **kwargs)


def servo_switch(name, *args, **kwargs):
    return d("servo_switch", name, *args, **kwargs)


def pwm(name, *args, **kwargs):
    return d("pwm", name, *args, **kwargs)


def rgb(name, *args, **kwargs):
    return d("rgb", name, *args, **kwargs)


def rgb_multi(name, *args, **kwargs):
    return d("rgb_multi", name, *args, **kwargs)


neopixel = rgb_multi


# ======== HT temperature/humidity with
def dht11(name, *args, **kwargs):
    return d("_ht", "DHT11", name, *args, **kwargs)


def dht22(name, *args, **kwargs):
    return d("_ht", "DHT22", name, *args, **kwargs)


def ds18x20(name, *args, **kwargs):
    return d("_ht", "DS18X20", name, *args, **kwargs)


def display(name, *args, **kwargs):
    return d("_display", "Display", name, *args, **kwargs)


def display44780(name, *args, **kwargs):
    return d("_display44780", "Display44780", name, *args, **kwargs)
