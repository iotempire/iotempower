# ulno shield01 

from machine import Pin, I2C
import ssd1306
import network

present = False

# test if lcd is responding
i2c = I2C(sda=Pin(4), scl=Pin(5))
try:
	display = ssd1306.SSD1306_I2C(128,64,i2c)
	display.fill(0)
	display.text("iot.ulno.net",16,4)
	display.show()
	
except OSError:
	# shield seems not to be here
	print("Ulno Shield 01 not found")
else:
	# shield is present
	present = True
	# configure leds
	d4=Pin(2,Pin.OUT)
	blue=d4 # TODO:reverse
	blue.high() # switch off, too bright
	d7=Pin(13,Pin.OUT)
	red=d7
	d8=Pin(15,Pin.OUT)
	yellow=d8
	# configure buttons
	d5=Pin(14,Pin.IN,Pin.PULL_UP)
	lower_button=d5
	d6=Pin(12,Pin.IN,Pin.PULL_UP)
	right_button=d6
	d3=Pin(0,Pin.IN,Pin.PULL_UP)
	left_button=d3
	#flash_button=d3

	# TODO: test if reset is requested
	if lower_button.value() == 0:
		# TODO show dialog
		print("Reset (lower button) is pressed.")
		import os
		os.remove("wifi_config.py")
		import machine
		machine.reset()

print("Network setup...")

# try network setup
try:
	import wifi_config
except ImportError:
	# not yet configured
	ap = network.WLAN(network.AP_IF)
	ap.active(True) # enable Accesspoint
	print("No wifi config, in ap mode.")
	print("Essid: "+ap.config('essid'))
	if present:
		display.text("To configure,",0,16)
		display.text("connect to:",0,24)
		display.text(ap.config("essid")[0:11],0,32)
		display.text(ap.config("essid")[11:],0,40)
		display.text("pw: micropythoN",0,48)
		display.text("Then run setup.",0,55)
		display.show()
else:
	import time
	print('Connecting to network...')
	print('Disabling AP.')
	ap = network.WLAN(network.AP_IF)
	ap.active(False) # disable Accesspoint
	sta_if = network.WLAN(network.STA_IF)
	time.sleep(1) # TODO: can this be reduced?
	#if not sta_if.isconnected():
	sta_if.active(True)
	sta_if.connect(wifi_config.wifi_name, wifi_config.wifi_pw)
	count=0
	while not sta_if.isconnected() and count<30:
		time.sleep_ms(500) # prevent busy waiting
		print("%s. waiting for connection"%count)
		# TODO: update on display
		count += 1
	print("connected")
	if sta_if.isconnected():
		print('network config:', sta_if.ifconfig())
		if present: # display network config
			display.text("WIFI connected.",0,16)
			display.text("IP:",0,32)
			display.text(sta_if.ifconfig()[0],0,40)
			display.text("Connect now via",0,48)
			display.text("iot_terminal IP",0,56)
			display.show()
	else:
		print("network connection unsuccessful")
		if present: # display network config
			display.text("WIFI not",0,16)
			display.text("connected.",0,32)
			display.text("Reconfig/reset!",0,48)
			display.show()
