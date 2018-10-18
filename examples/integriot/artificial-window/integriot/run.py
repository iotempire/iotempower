# offer some commands via mqtt to interact with the led grid of
# the artificial window

from neopixel import * # led strip code by https://github.com/jgarff/rpi_ws281x

import re

LED_COUNT = 90
LED_GPIO = 21
LED_FREQ = 800000  # LED signal frequency in hertz (usually 800khz)
LED_DMA = 10
LED_INVERT = False
LED_BRIGHTNESS = 255 # Set to 0 for darkest and 255 for brightest
LED_CHANNEL = 0
LED_STRIP_TYPE = ws.WS2811_STRIP_BRG
strip = Adafruit_NeoPixel(LED_COUNT, LED_GPIO, LED_FREQ, LED_DMA, LED_INVERT, LED_BRIGHTNESS, LED_CHANNEL, LED_STRIP_TYPE)

# Lines need to be swapped as the aluminium profiles angles the strips
# into opposite directions
matrix_map = [
    [ 72, 73, 74, 75, 76, 77, 78, 79, 80 ],
    [ 89, 88, 87, 86, 85, 84, 83, 82, 81 ],
    [ 54, 55, 56, 57, 58, 59, 60, 61, 62 ],
    [ 71, 70, 69, 68, 67, 66, 65, 64, 63 ],
    [ 36, 37, 38, 39, 40, 41, 42, 43, 44 ],
    [ 53, 52, 51, 50, 49, 48, 47, 46, 45 ],
    [ 18, 19, 20, 21, 22, 23, 24, 25, 26 ],
    [ 35, 34, 33, 32, 31, 30, 29, 28, 27 ],
    [  0,  1,  2,  3,  4,  5,  6,  7,  8 ],
    [ 17, 16, 15, 14, 13, 12, 11, 10,  9 ]
]

MATRIX_HEIGHT=len(matrix_map)
MATRIX_WIDTH=len(matrix_map[0])

def set_pixel(x, y, color):
    global strip, matrix_map
    nr = matrix_map[y][x]
    strip.setPixelColor( nr, color )

def get_pixel(x, y):
    global strip, matrix_map
    nr = matrix_map[y][x]
    return strip.getPixelColor(nr)

def fill(color):
    global strip, matrix_map
    for line in matrix_map:
        for nr in line:
            strip.setPixelColor( nr, color )

def clear():
    fill(Color(0,0,0))

def show():
    global strip
    strip.show()

# Intialize the library (must be called once before other functions).
def begin():
    global strip
    strip.begin()


# use integriot for mqtt integration
from integriot import *

animation = None

def animation_stop():
    global animation
    animation = None

def animation_next():
    global animation
    if animation:
        animation()

color_map = {
    "bright": Color(255,255,255),
    "black" : Color(0,0,0),
    "off"   : Color(0,0,0),
    "white" : Color(255,180,130),
    "on"    : Color(255,180,130),
    "red"   : Color(255,0,0),
    "green" : Color(0,255,0),
    "blue"  : Color(0,0,255),
    "yellow": Color(255,50,10),
}

def set_color(color, x=-1, y=-1):
    if isinstance(color, str):
        color = color.lower()
        if color in color_map:
            color = color_map[color]
        else:
            rgb = color.split(",")
            if len(rgb) == 3: # triple?
                try:
                    r = int(rgb[0])
                    g = int(rgb[1])
                    b = int(rgb[2])
                except:
                    pass
                else:
                    color = Color(r,g,b)
            else:
                if(len(color) == 6): # 6 digit hex?
                    try:
                        rgb = int("0x"+color, 16)
                    except:
                        pass
                    else:
                        b = rgb & 255
                        rgb //=  256
                        g = rgb & 255
                        rgb //= 256
                        r = rgb
                        color = Color(r,g,b)
    if isinstance(color, int):
        if x == -1 or y == -1:
            fill(color)
        else:
            set_pixel(x, y, color)




command_list = {
# "command":method
}

def matrix_cb(msg):
    animation_stop()
    # Accept commands to change what is displayed on the matrix
    command=msg.split()
    print("received: ", command)
    while len(command)>0:
        first = command[0].lower()
        if first in command_list:
            del command[0]
            processed = command_list[first](command)
            command = command[processed:]
        else: # color with coordinates given?
            x = -1
            y = -1
            try:
                x = int(command[0])
                y = int(command[1])
                del command[0]
                del command[0]
            except:
                pass
            set_color(command[0], x, y)
            del command[0]
    show()


# init and run integriot
init("localhost")

command_receiver = subscriber("led-matrix").subscribe(callback=matrix_cb)

begin()
clear()
show()

print("Ready to accept commands, ctrl-c to stop.")
# run integriot event loop
while True:
    process()  # call integriot loop
    animation_next() # call update
