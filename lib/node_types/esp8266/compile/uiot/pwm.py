# Simple servo control device
# author: ulno
# created: 2017-06-05
#
# based off: https://bitbucket.org/thesheep/micropython-servo/src/f562a6abeaf0e83b752838df7cd31d88ea10b2c7/servo.py?at=default&fileviewer=file-view-default

import machine
from uiot.device import Device


class PWM(Device):
    # Handle output devices
    def __init__(self, name, pin, freq=50, duty=0,
                 ignore_case=True, on_change=None,
                 report_change=False):
        self._duty = 0
        self._freq = freq
        Device.__init__(self, name, machine.PWM(pin, freq=freq, duty=duty),
                        setters={"freq/set": self.set_freq, "duty/set": self.set_duty},
                        getters={"freq": self.get_freq, "duty": self.get_duty},
                        ignore_case=ignore_case,
                        on_change=on_change, report_change=report_change)

    def turn(self, msg):
        if type(msg) in [str, int]:
            self.angle_list = [int(msg)]  # TODO: accept floats?
        else:  # should be a list
            self.angle_list = msg[:]
        self._trigger_next_turn()

    def set_duty(self, d):
        try:
            d = int(d)
        except:
            print("PWM: received invalid duty value:", d)
        else:
            self._duty = d
            self.pin.duty(d)

    def get_duty(self):
        return self._duty

    def set_freq(self, f):
        try:
            f = int(f)
        except:
            print("PWM: received invalid frequency value:", f)
        else:
            self._freq = f
            self.pin.freq(f)

    def get_freq(self):
        return self._freq

    def value(self):
        return self._duty, self._freq
