from machine import Pin
import time
import dht

# minimum ms between two reads
MEASURE_DELAY = 1000

# normal dht commands:
#d.measure()
#d.temperature()

d=None
_lasttime=none

def init(p):
    d=dht.DHT11(p)
    _lasttime = time.ticks_ms()
    d.measure()

def time_controlled_measure():
	global _lasttime, d
	
	newtime = time.ticks_ms()
	if newtime - _lasttime < 0 or newtime - _lasttime > MEASURE_DELAY:
		d.measure()
		_lasttime = newtime

def temperature():
    global d
    time_controlled_measure()
    return d.temperature()

def humidity():
    global d
    time_controlled_measure()
    return d.humidity()

# shortcuts
h=humidity
t=temperature
