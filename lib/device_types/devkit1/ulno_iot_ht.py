from machine import Pin
import time
import dht

# minimum ms between two reads
MEASURE_DELAY = 1000

# normal dht commands:
#d.measure()
#d.temperature()


d=dht.DHT11(Pin(5)) # pin 5 or d1 on ulno_iot_ht shield

_lasttime = time.ticks_ms()
d.measure()

def time_controlled_measure():
	global _lasttime
	
	newtime = time.ticks_ms()
	if newtime - _lasttime < 0 or newtime - _lasttime > MEASURE_DELAY:
		d.measure()
		_lasttime = newtime

def temperature():
    time_controlled_measure()
    return d.temperature()

def humidity():
    time_controlled_measure()
    return d.humidity()

# shortcuts
h=humidity
t=temperature
