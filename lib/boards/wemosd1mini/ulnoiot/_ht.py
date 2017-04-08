# humidity temperature
# author: ulno
# created: 2017-04-08
#

import time
from ulnoiot.device import Device

####### HT temperature/humidity with
class HT(Device):
    # minimum ms between two reads
    MEASURE_DELAY = 1000

    # Handle humidity and temperature from DS11
    def __init__(self, name, pin):
        Device.__init__(self, name, pin)
        import dht
        self.dht = dht.DHT11(pin)
        self.lasttime = time.ticks_ms()
        self.dht.measure()

    def time_controlled_measure(self):
        newtime = time.ticks_ms()
        if newtime - self.lasttime < 0 or newtime - self.lasttime > HT.MEASURE_DELAY:
            self.dht.measure()
            self.lasttime = newtime

    def temperature(self):
        self.time_controlled_measure()
        return self.dht.temperature()

    def humidity(self):
        self.time_controlled_measure()
        return self.dht.humidity()

    def value(self):
        return { "humidity": self.humidity(),
                 "temperature": self.temperature() }

    def _update(self):
        # trigger reading show eventually changed values
        self.time_controlled_measure()
