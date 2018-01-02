# humidity temperature dht base class
# author: ulno
# created: 2018-01-02
#

import time
from uiot.device import Device


####### HT temperature/humidity with
# TODO think about calibration
class _HTDHT(Device):
    # minimum ms between two reads
    delay = 1000

    # Handle humidity and temperature from dht devices
    def __init__(self, name, pin, dht_dev, delay,
                 on_change=None, report_change=True):
        self.delay = delay
        import dht
        self.dht = dht_dev
        self.lasttime = time.ticks_ms()
        self.dht.measure()
        Device.__init__(self, name, pin, on_change=on_change,
                        getters={"temperature": self.temperature,
                                 "humidity": self.humidity},
                        report_change=report_change)

    def time_controlled_measure(self):
        newtime = time.ticks_ms()
        if newtime - self.lasttime < 0 or newtime - self.lasttime > self.delay:
            self.dht.measure()
            self.lasttime = newtime

    def temperature(self):
        self.time_controlled_measure()
        return self.dht.temperature()

    def humidity(self):
        self.time_controlled_measure()
        return self.dht.humidity()

    def value(self):
        return {"humidity": self.humidity(),
                "temperature": self.temperature()}

    def _update(self):
        # trigger reading show eventually changed values
        self.time_controlled_measure()
