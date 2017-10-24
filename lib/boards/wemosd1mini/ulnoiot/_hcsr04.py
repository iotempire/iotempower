# HCSR04 Device
# author: cmuck, ktakač
# created: 2017-10-23

from machine import Pin
from ulnoiot.device import Device
from time import sleep_us,ticks_us,ticks_diff

####### HCSR04 Distance sensor
class HCSR04(Device):
    def __init__(self, name, trigger_pin, echo_pin, precision=1,
                 on_change=None, report_change=True):
        self.current_value = 0
        self.trigger_pin = trigger_pin
        self.echo_pin = echo_pin
        self.precision = precision
        Device.__init__(self, name, echo_pin, on_change=on_change,
                        report_change=report_change)
        self.getters[""] = self.value
        self.trigger_pin.init(Pin.OUT)
        self.echo_pin.init(Pin.IN)

    def value(self):
        return self.current_value  # just return value

    def _update(self):
        value = self.measure()
        if abs(value-self.current_value) >= self.precision:
            self.current_value = value

    def measure(self):
        sleep_us(5)
        self.trigger_pin.on()
        sleep_us(10)
        self.trigger_pin.off()

        while not self.echo_pin():
            pass
        start = ticks_us()

        while self.echo_pin():
            pass
        delta = ticks_diff(ticks_us(), start)

        return delta / 58.8