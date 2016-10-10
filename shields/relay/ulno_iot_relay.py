from machine import Pin

d1=Pin(5,Pin.OUT)

def left():
    d1.low()
off=left

def right():
    d1.high()
on=right
