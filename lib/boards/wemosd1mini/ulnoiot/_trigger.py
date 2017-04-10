# Trigger Device
# author: ulno
# created: 2017-04-08
# detect falling or/and rising edges on gpio inputs

from machine import Pin
from ulnoiot.device import Device

####### simple Input, contact devices/push buttons
class Trigger(Device):
    OVERFLOW=1000000
    # Handle contact or button like devices, if both false-> rising
    def __init__(self, name, pin,
                 rising=False, falling=False,
                 pullup=True):
        Device.__init__(self, name, pin)
        if pullup:
            pin.init(Pin.IN,Pin.PULL_UP)
        else:
            pin.init(Pin.IN,Pin.OPEN_DRAIN)
        trigger = None
        if rising and falling:
            trigger=Pin.IRQ_RISING|Pin.IRQ_FALLING
        elif not rising and falling:
            trigger = Pin.IRQ_FALLING
        else: # also if both all false
            trigger = Pin.IRQ_RISING
        pin.irq(trigger=trigger, handler=self.callback)
        self.counter = 0
        self.report_counter = 0
        self.triggered = False

    def callback(self,p):
        self.triggered = True
        self.counter += 1
        if self.counter >= Trigger.OVERFLOW:
            self.counter = 0

    def value(self):
        return self.report_counter

    def _update(self):
        if self.triggered:
            self.report_counter = self.counter
            self.triggered = False
