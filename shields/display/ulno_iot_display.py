# ulno_iot display

from machine import Pin, I2C
import ssd1306

present = False

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

# TODO: define easier output functions which can scroll
