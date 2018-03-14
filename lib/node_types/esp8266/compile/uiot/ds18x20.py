# temperature based on ds18x20
# author: ulno
# created: 2018-01-02
#

import time
import gc
from uiot.device import Device

class DS18X20(Device):
    MEASURE_DELAY = 750

    # Handle temperature from sd18x20 device
    def __init__(self, name, pin, on_change=None, filter=None):
        import onewire, ds18x20
        gc.collect()
        self.ds = ds18x20.DS18X20(onewire.OneWire(pin))
        self.roms = self.ds.scan()
        self.lasttime = time.ticks_ms()
        self.ds.convert_temp()
        self.temp_list = None
        Device.__init__(self, name, pin, on_change=on_change, filter=filter)

    def measure(self):
        newtime = time.ticks_ms()
        if newtime - self.lasttime < 0 or newtime - self.lasttime > DS18X20.MEASURE_DELAY:
            self.temp_list = []
            for rom in self.roms:
                self.temp_list.append(self.ds.read_temp(rom))
            if len(self.temp_list) == 1:
                self.temp_list = self.temp_list[0]
            self.ds.convert_temp()
            self.lasttime = newtime
        return self.temp_list
