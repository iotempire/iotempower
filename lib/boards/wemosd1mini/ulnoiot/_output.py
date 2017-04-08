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
    def __init__(self, name, pin, *args, high_command='on', low_command='off', ignore_case=True):
        if len(args) > 0:
            high_command = args[0]
            if len(args) > 1:
                low_command = args[1]
        Device.__init__(self, name, pin,
                        settable=True, ignore_command_case=ignore_case,
                        value_map={1: high_command.encode(), 0: low_command.encode()})
        pin.init(Pin.OUT)
        self.add_command(high_command, self.high)
        self.add_command(low_command, self.low)

    def high(self):
        self.pin.high()

    def low(self):
        self.pin.low()

    on = high
    off = low

    def value(self):
        return self.pin()
