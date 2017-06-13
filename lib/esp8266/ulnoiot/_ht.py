# humidity temperature
# author: ulno
# created: 2017-04-08
#

import time
import gc
from ulnoiot.device import Device

####### HT temperature/humidity with
# TODO think about calibration
class _HTDHT(Device):
    # minimum ms between two reads
    delay = 1000

    # Handle humidity and temperature from dht devices
    def __init__(self, name, pin, dht_dev, delay,
                 on_change=None,report_change=True):
        self.delay = delay
        import dht
        self.dht = dht_dev
        self.lasttime = time.ticks_ms()
        self.dht.measure()
        Device.__init__(self, name, pin, on_change=on_change,
                        getters={"temperature":self.temperature,
                                 "humidity":self.humidity},
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
        return { "humidity": self.humidity(),
                 "temperature": self.temperature() }

    def _update(self):
        # trigger reading show eventually changed values
        self.time_controlled_measure()

class DHT11(_HTDHT):
    def __init__(self, name, pin, on_change=None):
        import dht
        _HTDHT.__init__(self, name, pin, dht.DHT11(pin), 1000,
                        on_change=on_change)

class DHT22(_HTDHT):
    def __init__(self, name, pin, on_change=None):
        # TODO: also handle an I2C object here
        import dht
        _HTDHT.__init__(self, name, pin, dht.DHT22(pin), 2000,
                        on_change=on_change)

class DS18X20(Device):
    MEASURE_DELAY = 750

    # Handle humidity and temperature from dht devices
    def __init__(self, name, pin, on_change=None):
        import onewire, ds18x20
        gc.collect()
        Device.__init__(self, name, pin,on_change=on_change)
        self.ds = ds18x20.DS18X20(onewire.OneWire(pin))
        self.roms = self.ds.scan()
        self.lasttime = time.ticks_ms()
        self.ds.convert_temp()
        self.temp_list = None

    def time_controlled_measure(self):
        newtime = time.ticks_ms()
        if newtime - self.lasttime < 0 or newtime - self.lasttime > DS18X20.MEASURE_DELAY:
            self.temp_list = []
            for rom in self.roms:
                self.temp_list.append(self.ds.read_temp(rom))
            if len(self.temp_list)==1:
                self.temp_list = self.temp_list[0]
            self.ds.convert_temp()
            self.lasttime = newtime

    def value(self):
        return self.temp_list

    def _update(self):
        # trigger reading show eventually changed values
        self.time_controlled_measure()
