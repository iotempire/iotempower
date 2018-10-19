# offer some commands via mqtt to interact with the led grid of
# the artificial window

from PIL import Image # Pillow library
from neopixel import * # led strip code by https://github.com/jgarff/rpi_ws281x
from random import randint
from time import time
import os

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


white = (255, 180, 130)

def int2rgb(colint):
    b = colint & 255
    colint >>= 8
    g = colint & 255
    colint >>= 8
    return colint, g, b


def CColor(r, g=-1, b=-1, calibrate=white):
    if g==-1:
        r, g, b = int2rgb(r)
    # calibrated color
    return Color(r*calibrate[0]//255, g*calibrate[1]//255, b*calibrate[2]//255)


def set_pixel(x, y, color):
    global strip, matrix_map
    x = min(MATRIX_WIDTH,max(0,x))
    y = min(MATRIX_HEIGHT,max(0,y))
    nr = matrix_map[y][x]
    strip.setPixelColor(nr, color)


def get_pixel(x, y):
    global strip, matrix_map
    nr = matrix_map[y][x]
    return strip.getPixelColor(nr)


def draw_image(im, startx=0, starty=0, calibrate=(255,255,255)):
    imw,imh = im.size
    for y in range(MATRIX_HEIGHT):
        imx = startx
        imy = starty+y
        for x in range(MATRIX_WIDTH):
            set_pixel(x, y, CColor(*im.getpixel((imx%imw, imy%imh)),calibrate=calibrate))
            imx += 1


def get_image():
    im = Image.new("RGB",(MATRIX_WIDTH,MATRIX_HEIGHT))
    for y in range(MATRIX_HEIGHT):
        for x in range(MATRIX_WIDTH):
            im.putpixel((x, y), int2rgb(get_pixel(x, y)))
    return im


def fill(color):
    global strip, matrix_map
    for line in matrix_map:
        for nr in line:
            strip.setPixelColor(nr, color)


def clear():
    fill(Color(0,0,0))


def show():
    global strip
    strip.show()


# Initialize the LED-Strip library (must be called once before other functions).
def begin():
    global strip
    strip.begin()


animation_fps = 30
animation_framelen = 1.0 / animation_fps
animation_lasttime = time()

animation = None
animation_frames = 0
animation_frame_show_count = 1
animation_image_backup = None


def animation_stop():
    global animation, animation_image_backup, animation_frames
    animation = None
    animation_frames = 0
    # restore image after animation
    if animation_image_backup is not None:
        draw_image(animation_image_backup)
        show()
    animation_image_backup = None


def animation_start(animation_cb, length):
    global animation, animation_image_backup, animation_frames
    animation = animation_cb
    animation_frames = length
    animation_image_backup = get_image()


def animation_next():
    global animation, animation_lasttime, animation_frames
    while time()-animation_lasttime >= animation_framelen:
        if animation is not None and animation_frames > 0:
            animation()
            show()
            animation_frames -= 1
            if animation_frames <= 0:
                animation_stop()
        animation_lasttime += animation_framelen


color_map = {
    "bright": Color(255,255,255),
    "black" : Color(0,0,0),
    "off"   : Color(0,0,0),
    "white" : Color(*white),
    "on"    : Color(*white),
    "red"   : Color(255,0,0),
    "green" : Color(0,255,0),
    "blue"  : Color(0,0,255),
    "yellow": Color(255,90,0),
    "gray"  : Color(127,90,65),
    "grey"  : Color(127,90,65),
    "darkgray": Color(32, 22, 16),
    "darkgrey": Color(23, 22, 16),
}


def read_color(color):
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
                        r, g, b = int2rgb(rgb)
                        color = Color(r,g,b)
    if isinstance(color, int):
        return color
    return None


def set_color(color, x=-1, y=-1):
    color = read_color(color)
    if  color is not None:
        if x == -1 or y == -1:
            fill(color)
        else:
            set_pixel(x, y, color)


def command_row(params):
    if len(params)<2:
        return len(params)
    try:
        y = int(params[0])
    except:
        pass
    else:
        for x in range(MATRIX_WIDTH):
            set_color(params[1], x, y)
    return 2


def command_column(params):
    if len(params)<2:
        return len(params)
    try:
        x = int(params[0])
    except:
        pass
    else:
        for y in range(MATRIX_HEIGHT):
            set_color(params[1], x, y)
    return 2


def draw_lightning(x):
    c=Color(*white)
    for y in range(MATRIX_HEIGHT):
        r = randint(0, 2)
        x += r - 1
        if x<0: x=0
        elif x>=MATRIX_WIDTH: x = MATRIX_WIDTH - 1
        set_pixel(x,y,c)


def animation_lightning():
    global lightning_column, animation_image_backup
#    set_color("darkgray")
    draw_image(animation_image_backup, calibrate=(64, 64, 64))  # draw current background in dark
    draw_lightning(lightning_column)



def command_lightning(_):
    global lightning_column
    lightning_column = randint(0, MATRIX_WIDTH)
    animation_start(animation_lightning, randint(animation_fps // 3, animation_fps))


def animation_blood_smear():
    pass


def command_blood_smear(_):
    animation_start(animation_blood_smear, 0)


imagelist = []
image_calibration = white


def animation_images():
    global animation_frames, imagelist, animation_frame_show_count
    l = len(imagelist)
    if animation_frames % animation_frame_show_count == 0:
        im = imagelist[l - animation_frames//animation_frame_show_count]
        draw_image(im, calibrate=image_calibration)
    if animation_frames <= 1:
        animation_frames = l



def command_images(args):
    global imagelist, image_calibration, animation_frame_show_count
    consumed = 0
    # check if we got a calibration color first
    if len(args) == 0:
        return consumed
    color = read_color(args[0])
    if color is None:  # did we get a calibration color?
        image_calibration = white
    else:
        image_calibration = int2rgb(color)
        consumed += 1
        args = args[1:]
    # check if we get a playlength in s as integer next
    if len(args) == 0:
        return consumed
    animation_cycle_length = None
    try:
        animation_cycle_length = float(args[0])
    except:
        pass
    if animation_cycle_length is not None:
        consumed += 1
        args = args[1:]
    # check for folder
    if len(args) == 0:
        return consumed
    # this should be a folder
    folder = args[0]
    imagelist=[]
    try:
        for f in os.listdir(folder):
            imagelist.append(Image.open(os.path.join(folder, f)))
    except:
        pass
    if animation_cycle_length is None:
        animation_frame_show_count = 1  # show each frame once
    else:
        animation_frame_show_count = int(animation_cycle_length*animation_fps/len(imagelist)+0.1)
    consumed += 1
    animation_start(animation_images, len(imagelist)*animation_frame_show_count+1)  # we will keep this higher to refill when down
    return consumed  # consume arguments


def animation_halloween():
    pass


def command_halloween(_):
    animation_start(animation_halloween, 0)


def animation_sunrise():
    pass

def command_sunrise(_):
    animation_start(animation_sunrise, 0)


def animation_sunset():
    pass


def command_sunset(_):
    animation_start(animation_sunset, 0 )


def animation_daytime():
    pass


def command_daytime(_):
    animation_start(animation_daytime, 0)



command_list = {
    # "command":method
    "row": command_row,
    "column": command_column,
    "lightning": command_lightning,
    "halloween": command_halloween,
    "blood_smear": command_blood_smear,
    "sunrise": command_sunrise,
    "sunset": command_sunset,
    "daytime": command_daytime,
    "images": command_images,
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
            if processed is None:
                processed = 0
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


# use integriot for mqtt integration
from integriot import *

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
