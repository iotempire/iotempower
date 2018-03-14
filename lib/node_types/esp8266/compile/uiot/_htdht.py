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
                 on_change=None, report_change=True,
                 filter=None):
        self.delay = delay
        import dht
        self.dht = dht_dev
        self.lasttime = time.ticks_ms()
        self.dht.measure()
        Device.__init__(self, name, pin, on_change=on_change,
                        getters={"temperature": self._get_t,
                                 "humidity": self._get_h},
                        report_change=report_change,
                        filter=filter)

    def measure(self):
        newtime = time.ticks_ms()
        if newtime - self.lasttime < 0 or newtime - self.lasttime > self.delay:
            try:
                self.dht.measure()
                self.lasttime = newtime
            except OSError:
                print("Warning: Trouble measuring (timeout?).")
                return None
        return self.dht.humidity(), self.dht.temperature()

    def _get_t(self):
        return self.value()[1]

    def _get_h(self):
        return self.value()[0]
