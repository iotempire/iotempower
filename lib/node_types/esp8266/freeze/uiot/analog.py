# Analog Device
# author: ulno
# created: 2017-04-28


from machine import ADC
from uiot.device import Device


class Analog(Device):
    # Handle devices connected to the analog port
    # offers a digital mode through using threshold
    def __init__(self, name, precision=1, threshold=None,
                 on_change=None, report_change=True, filter=None):
        self.precision = precision
        self.threshold = None
        if threshold is not None:
            self.threshold = max(1, min(threshold, 1023))
        self.last_value = None
        Device.__init__(self, name, ADC(0), on_change=on_change,
                        report_change=report_change, filter=filter)

    def measure(self):
        value = self.port.read()
        if self.last_value is None \
                or abs(value - self.last_value) >= self.precision:
            self.last_value = value
            if self.threshold is None:
                return self.last_value  # just return value
            else:
                if self.last_value > self.threshold - self.precision: # behave like a digital sensor
                    return 1
                else:
                    return 0
        return self.last_value
