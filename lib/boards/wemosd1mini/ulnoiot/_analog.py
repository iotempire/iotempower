# Analog Device
# author: ulno
# created: 2017-04-28


from machine import ADC
from ulnoiot.device import Device


####### simple Input, contact devices/push buttons
class Analog(Device):
    # Handle analog input devices
    OVERFLOW=1000000
    # Handle contact or button like devices, if both false-> rising
    def __init__(self, name, precision=1):
        Device.__init__(self, name, ADC(0))

    def callback(self,p):
        self.triggered = True
        self.counter += 1
        if self.counter >= Trigger.OVERFLOW:
            self.counter = 0

    def value(self):
        return self.pin.read()
