# Simple output device
# author: ulno
# created: 2017-04-08
#

import time
from machine import Pin
from ulnoiot.device import Device

####### simple LEDs, other Output (switches)
class Output(Device):
    # Handle output devices
    def __init__(self, name, pin, *args, high_command='on', low_command='off',
                 ignore_case=True, on_change=None):
        if len(args) > 0:
            high_command = args[0]
            if len(args) > 1:
                low_command = args[1]
        self.high_command=high_command
        self.low_command=low_command
        Device.__init__(self, name, pin,
                        setters={"set":self.evaluate}, ignore_command_case=ignore_case,
                        on_change = on_change)
        pin.init(Pin.OUT)

    def evaluate(self,msg):
        if msg==self.high_command:
            self.high()
        elif msg == self.low_command:
            self.low()

    def high(self):
        self.pin.high()

    def low(self):
        self.pin.low()

    on = high
    off = low

    def value(self):
        return self.pin()
