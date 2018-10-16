# NeoPixel library strandtest example
# Author: Tony DiCola (tony@tonydicola.com)
#
# Direct port of the Arduino NeoPixel library strandtest example.  Showcases
# various animations on a strip of NeoPixels.
import time
import sys

from neopixel import *
from PIL import Image

LED_FREQ = 800000  # LED signal frequency in hertz (usually 800khz)
LED_BRIGHTNESS = 63 # Set to 0 for darkest and 255 for brightest
LED_STRIP_TYPE = ws.WS2811_STRIP_GRB
STRIP_COUNT=4
strip=range(STRIP_COUNT)
strip[0] = Adafruit_NeoPixel(20, 21, LED_FREQ, 5, False, LED_BRIGHTNESS, 0, LED_STRIP_TYPE)
strip[1] = Adafruit_NeoPixel(20, 13, LED_FREQ, 6, False, LED_BRIGHTNESS, 1, LED_STRIP_TYPE)
strip[2] = Adafruit_NeoPixel(40, 10, LED_FREQ, 7, False, LED_BRIGHTNESS, 0, LED_STRIP_TYPE)
strip[3] = Adafruit_NeoPixel(80, 18, LED_FREQ, 8, False, LED_BRIGHTNESS, 0, LED_STRIP_TYPE)

matrix = [
[ (3,60), (3,61), (3,62), (3,63), (3,64), (3,65), (3,66), (3,67), (3,68), (3,69), (3,70), (3,71), (3,72), (3,73), (3,74), (3,75), (3,76), (3,77), (3,78), (3,79) ],
[ (3,59), (3,58), (3,57), (3,56), (3,55), (3,54), (3,53), (3,52), (3,51), (3,50), (3,49), (3,48), (3,47), (3,46), (3,45), (3,44), (3,43), (3,42), (3,41), (3,40) ],
[ (3,20), (3,21), (3,22), (3,23), (3,24), (3,25), (3,26), (3,27), (3,28), (3,29), (3,30), (3,31), (3,32), (3,33), (3,34), (3,35), (3,36), (3,37), (3,38), (3,39) ],
[ (3,19), (3,18), (3,17), (3,16), (3,15), (3,14), (3,13), (3,12), (3,11), (3,10), (3, 9), (3, 8), (3, 7), (3, 6), (3, 5), (3, 4), (3, 3), (3, 2), (3, 1), (3, 0) ],
[ (2,20), (2,21), (2,22), (2,23), (2,24), (2,25), (2,26), (2,27), (2,28), (2,29), (2,30), (2,31), (2,32), (2,33), (2,34), (2,35), (2,36), (2,37), (2,38), (2,39) ],
[ (2,19), (2,18), (2,17), (2,16), (2,15), (2,14), (2,13), (2,12), (2,11), (2,10), (2, 9), (2, 8), (2, 7), (2, 6), (2, 5), (2, 4), (2, 3), (2, 2), (2, 1), (2, 0) ],
[ (1,19), (1,18), (1,17), (1,16), (1,15), (1,14), (1,13), (1,12), (1,11), (1,10), (1, 9), (1, 8), (1, 7), (1, 6), (1, 5), (1, 4), (1, 3), (1, 2), (1, 1), (1, 0) ],
[ (0,19), (0,18), (0,17), (0,16), (0,15), (0,14), (0,13), (0,12), (0,11), (0,10), (0, 9), (0, 8), (0, 7), (0, 6), (0, 5), (0, 4), (0, 3), (0, 2), (0, 1), (0, 0) ],
]
MATRIX_HEIGHT=len(matrix)
MATRIX_WIDTH=len(matrix[0])


def set_pixel( x, y, color ):
    global strip
    s,nr = matrix[y][x]
    strip[s].setPixelColor( nr, color )

def fill( color ):
    global strip
    for line in matrix:
        for s,nr in line:
            strip[s].setPixelColor( nr, color )

def clear():
    fill( Color(0,0,0) )

def show():
    global strip
    for s in range(STRIP_COUNT):
        strip[s].show()

# Intialize the library (must be called once before other functions).
def begin():
    global strip
    for s in range(STRIP_COUNT):
        strip[s].begin()

def draw_image(im,startx,starty):
    imw,imh = im.size
    for y in range(MATRIX_HEIGHT):
        imx = startx
        imy = starty+y
        for x in range(MATRIX_WIDTH):
            set_pixel(x, y, Color(*im.getpixel((imx%imw, imy%imh))))
            imx += 1

# Main program logic follows:
if __name__ == '__main__':
    begin()

    print("filename:", sys.argv[1])
#    try:
    im = Image.open(sys.argv[1])
#    except:
#        if len(sys.argv)==0:
#            raise Exception("Please provide an image filename as a parameter.")
#    else:
#        raise Exception("Image file %s could not be loaded" % sys.argv[1])

    print ('Press Ctrl-C to quit.')

    # Black out any LEDs that may be still on for the last run
    clear()

#    c = Color(0,0,255)
#    black = Color(0,0,0)
#    for loop in range(1000):
#        col = loop % 20
#        lastcol = (loop + 19) % 20
#        for i in range(8):
#            set_pixel( lastcol, i, black )
#            set_pixel( col, i, c )
#        show()
#        time.sleep(0.03)

    for x in range(10000):
        draw_image(im,x,0)
        show()
        time.sleep(0.01)
