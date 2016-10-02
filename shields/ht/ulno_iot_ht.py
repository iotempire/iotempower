from machine import Pin
import dht
d=dht.DHT11(Pin(5))

#d.measure()
#d.temperature()

def temperature():
    d.measure()
    return d.temperature()

def humidity():
    d.measure()
    return d.humidity()
