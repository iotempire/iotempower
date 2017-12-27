# Analog Device
# author: ulno
# created: 2017-04-28


from machine import ADC
from ulnoiot.device import Device


####### simple Input, contact devices/push buttons
class Analog(Device):
    # Handle analog input devices
    OVERFLOW = 1000000

    # Handle contact or button like devices, if both false-> rising
    def __init__(self, name, precision=1, threshold=None,
                 on_change=None, report_change=True):
        self.precision = precision
        self.threshold = None
        if threshold is not None:
            self.threshold = max(1, min(threshold, 1023))
        self.current_value = -10000
        Device.__init__(self, name, ADC(0), on_change=on_change,
                        report_change=report_change)
        self.getters[""] = self.value

    def callback(self, p):
        self.triggered = True
        self.counter += 1
        if self.counter >= Trigger.OVERFLOW:
            self.counter = 0

    def value(self):
        if self.threshold is None:
            return self.current_value  # just return value
        else:
            if self.current_value > self.threshold - self.precision:
                return 1
            else:
                return 0

    def _update(self):
        # Needs to be read in a polling scenario on a regular basis (very frequent)
        value = self.pin.read()
        if abs(value - self.current_value) >= self.precision:
            self.current_value = value

    # TODO: allow some averaging over several values
