# offer some commands via mqtt to interact with the led grid of
# the artificial window

from neopixel import * # led strip code by https://github.com/jgarff/rpi_ws281x


LED_FREQ = 800000  # LED signal frequency in hertz (usually 800khz)
LED_BRIGHTNESS = 63 # Set to 0 for darkest and 255 for brightest
LED_STRIP_TYPE = ws.WS2811_STRIP_BRG
STRIP_COUNT=4
strip = Adafruit_NeoPixel(20, 21, LED_FREQ, 5, False, LED_BRIGHTNESS, 0, LED_STRIP_TYPE)

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

def set_pixel( x, y, color ):
    global strip, matrix_map
    nr = matrix_map[y][x]
    strip.setPixelColor( nr, color )

def fill( color ):
    global strip, matrix_map
    for line in matrix_map:
        for nr in line:
            strip.setPixelColor( nr, color )

def clear():
    fill( Color(0,0,0) )

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

def bright( _ ):
    animation_stop()
    fill(Color(255,255,255))
    show()

def black( _ ):
    animation_stop()
    clear()
    show()

def natural_white( _ ):
    animation_stop()
    fill(Color(255,180,130))
    # better?
    # fill(Color(255,120,50))
    show()

def red( _ ):
    animation_stop()
    fill(Color(255,0,0))
    show()

def green( _ ):
    animation_stop()
    fill(Color(0,255,0))
    show()

def blue( _ ):
    animation_stop()
    fill(Color(0,0,255))
    show()

def yellow( _ ):
    animation_stop()
    fill(Color(255,40,5))
    show()


command_list = {
    "bright": bright,
    "black": black,
    "off": black,
    "white": natural_white,
    "on": natural_white,
    "red": red,
    "green": green,
    "blue": blue,
    "yellow": yellow,
}

def matrix_cb(msg):
    # Accept commands to change what is displayed on the matrix

    command=msg.split()
    print("received: ", command)
    if len(command)>0:
        first = command[0].lower()
        if first in command_list:
            command_list[first](command[1:])



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
