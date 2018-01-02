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
                 ignore_case=True, on_change=None, report_change=False):
        if len(args) > 0:
            high_command = args[0]
            if len(args) > 1:
                low_command = args[1]
        if ignore_case:
            high_command = high_command.lower()
            low_command = low_command.lower()
        self.high_command = high_command
        self.low_command = low_command
        Device.__init__(self, name, pin,
                        setters={"set": self.evaluate}, ignore_case=ignore_case,
                        on_change=on_change, report_change=report_change,
                        value_map={1: self.high_command, 0: self.low_command})
        pin.init(Pin.OUT)

    def evaluate(self, msg):
        if msg == self.high_command:
            self.on()
        elif msg == self.low_command:
            self.off()

    def high(self):
        self.pin.on()

    def low(self):
        self.pin.off()

    on = high
    off = low

    def value(self):
        return self.pin()
