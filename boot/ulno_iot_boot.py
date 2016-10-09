# ulno_iot boot-script
# Try to load display and give some instructions

import network
import ulno_iot_display as display

wifi_configured = False

if display.present: # needs to also be present for reset to work
	# test if reset is requested
	from machine import Pin
	p = Pin(12,Pin.IN) # TODO: make some kind of Pin configuration file for different platforms
	if p.value() == 0:
		# TODO: show dialog on screen
		print("Reset (right button) is pressed - keep pressed to reset.")
		from ulno_iot_devel import red,yellow,blue
		# turn on all leds
		red.high()
		yellow.high()
		blue.low()
		import time
		time.sleep(3)
		if p.value() == 0:
			print("Reset (right button) is still pressed - reseting.")
			red.low()
			yellow.low()
			blue.high()
			import os
			try:
				os.remove("wifi_config.py")
			except OSError:
				pass
			time.sleep(2)
			import machine
			machine.reset()

print("Network setup...")

# try network setup (also without display)
try:
	import wifi_config
except ImportError:
	# not yet configured
	ap = network.WLAN(network.AP_IF)
	ap.active(True) # enable Accesspoint
	print("No wifi config, in ap mode.")
	print("Essid: "+ap.config('essid'))
	if display.present:
		display.text("To configure,",0,16)
		display.text("connect to:",0,24)
		display.text(ap.config("essid")[0:11],0,32)
		display.text(ap.config("essid")[11:],0,40)
		display.text("pw: micropythoN",0,48)
		display.text("copy wifi_cfg.py",0,55)
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
	if sta_if.isconnected():
		print("connected")
		print('network config:', sta_if.ifconfig())
		wifi_configured = True
		if display.present: # display network config
			display.text("WIFI connected.",0,16)
			display.text("IP:",0,32)
			display.text(sta_if.ifconfig()[0],0,40)
			display.text("Connect now via",0,48)
			display.text("iot_terminal IP",0,56)
			display.show()
	else:
		print("network connection unsuccessful")
		if display.present: # display network config
			display.text("WIFI not",0,16)
			display.text("connected.",0,32)
			display.text("Reconfig/reset!",0,48)
			display.show()
