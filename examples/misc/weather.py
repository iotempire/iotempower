# small example polling and displaying the temperature
import time
import ujson
from urllib import urequest
import ulno_iot_display as display
from ulno_iot_devel import left_button,right_button,lower_button
# attention: create a file weather_config.py
# in that file have a line appid="yourid"
from weather_config import appid

## brattleboro
#lat=42.850922
#lon=-72.557869
# hacdc
lat=38.9335502
lon=-77.0380426

url = "http://api.openweathermap.org/data/2.5/weather?lon=%s&lat=%s&APPID=%s"%(lon,lat,appid)

lastjson = ""
lasttime = time.ticks_ms()

def get():
	global lasttime
	
	newtime = time.ticks_ms()
	if len(lastjson) < 20 or newtime-lasttime>60000: # nothing read yet or at least one minute pass
		r=urequest.urlopen(url)
		j=ujson.loads(r.read(2000))
		r.close()
		lasttime=newtime
	else:
		j=lastjson
	return j

def loop():
	show_temp=True
	while True:
		g=get()
		display.fill(0)
		display.text("Weather in",0,0)
		display.text(g['name'],0,8)
		if left_button.value()==0 or right_button.value()==0:
			show_temp=not show_temp
		if show_temp:
			celsius=g['main']['temp']-273.15
			display.text("Temp/C: %.1f"%celsius,0,24)
			display.text("Temp/F: %.1f"%(celsius*1.8+32),0,32)
		else: # humidity
			display.text("Humidity: %d%%"%g['main']['humidity'],0,24)
		if lower_button.value()==0:
			break
		display.show()
		time.sleep_ms(500)

loop()
