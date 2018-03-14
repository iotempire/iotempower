# Trigger Device
# author: ulno
# created: 2017-04-08
# detect falling or/and rising edges on gpio inputs

from machine import Pin
from uiot.device import Device


####### simple Input, contact devices/push buttons
class Trigger(Device):
    OVERFLOW = 1000000

    # Handle contact or button like devices, if both false-> rising
    def __init__(self, name, pin,
                 rising=False, falling=False,
                 pullup=True, on_change=None, report_change=True):
        if pullup:
            pin.init(Pin.IN, Pin.PULL_UP)
        else:
            pin.init(Pin.IN, Pin.OPEN_DRAIN)
        if rising and falling:
            trigger = Pin.IRQ_RISING | Pin.IRQ_FALLING
        elif not rising and falling:
            trigger = Pin.IRQ_FALLING
        else:  # also if both all false
            trigger = Pin.IRQ_RISING
        pin.irq(trigger=trigger, handler=self._cb)
        self.counter = 0
        self.report_counter = 0
        self.triggered = False
        Device.__init__(self, name, pin, on_change=on_change,
                        report_change=report_change)
        self.getters[""] = self.value

    def _cb(self, p):
        self.triggered = True
        self.counter += 1
        if self.counter >= Trigger.OVERFLOW:
            self.counter = 0

    def measure(self):
        if self.triggered:
            self.report_counter = self.counter
            self.triggered = False
        return self.report_counter
