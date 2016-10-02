# ulno_iot display

from machine import Pin, I2C
import ssd1306

CHAR_WIDTH = 128//8
CHAR_HEIGHT = 64//8

present = False
_y = 0
_x = 0

# test if lcd is responding
i2c = I2C(sda=Pin(4), scl=Pin(14))
try:
	display = ssd1306.SSD1306_I2C(128,64,i2c)
	display.fill(0)
	display.text("iot.ulno.net",16,4)
	display.show()
	
except OSError:
	# shield seems not to be here
	print("ulno_iot-lcd not found")
else:
	# shield is present
	present = True
	text = display.text
	show = display.show
	fill = display.fill
	scroll = display.scroll
	pixel = display.pixel
	

### easier output functions which can scroll

def set_cursor( x, y ):
	global _x,_y

	if x < 0: x = 0
	if y < 0: y = 0
	if x >= CHAR_WIDTH: x = CHAR_WIDTH - 1
	if y >= CHAR_HEIGHT: y = CHAR_HEIGHT - 1
	_x = x
	_y = y

def get_cursor():
	global _x, _y
	return ( _x,_y )
	
# clear display immediately
def clear():
	set_cursor( 0, 0 )
	display.fill(0)
	display.show()

# move cursor down and scroll the text area by one line if at screen end
def line_feed(show=True):
	global _x,_y
	
	if(_y<CHAR_HEIGHT-1):
		_y += 1
	else:
		display.scroll( 0, -8 )
		if show:
			display.show()
	_x = 0
	
# move just to start of line and clear the whole line
def clear_line(show=True):
	global _x, _y
	_x=0
	# clear line
	for y in range(_y*8,(_y+1)*8):
		for x in range(0, CHAR_WIDTH * 8):
			display.pixel(x,y,False)
	if show:
		display.show()
			

# print some text in the text area and linebreak and wrap if necessary
def print(text="",newline=False):
	global _x
	
	linefeed_last = text.endswith("\n")
	if linefeed_last:
		text=text[:-1]
	l_first = True
	for l in text.split("\n"):
		if not l_first: # scroll if it's not the first line
			line_feed(show=False)
		l_first = False
		while len(l) > 0:
			sub = l[0:CHAR_WIDTH-_x]
			display.text(sub,_x*8,_y*8)
			_x += len(sub)
			if _x >= CHAR_WIDTH:
				line_feed(show=False)
			l = l[len(sub):]
	if linefeed_last:
		line_feed(show=False)
	if newline:
		line_feed(show=False)
	display.show()
	
def println(text=""):
	print(text,newline=True)


