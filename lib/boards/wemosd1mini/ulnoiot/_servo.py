# Simple servo control device
# author: ulno
# created: 2017-06-05
#
# based off: https://bitbucket.org/thesheep/micropython-servo/src/f562a6abeaf0e83b752838df7cd31d88ea10b2c7/servo.py?at=default&fileviewer=file-view-default

import time
from machine import PWM
from ulnoiot.device import Device

class Servo(Device):
    # Handle output devices
    def __init__(self, name, pin,
                 ignore_case=True, on_change=None,
                 report_change=False,
                 turn_time_ms=700,
                 freq=50, min_us=600, max_us=2400, angle=180):
        self.min_us = min_us
        self.max_us = max_us
        self.us = 0
        self.freq = freq
        self.angle = angle
        self.angle_list=None
        self.turn_time_ms=turn_time_ms
        self.turn_start=None
        Device.__init__(self, name, PWM(pin, freq=self.freq, duty=0),
                        setters={"set":self.turn}, ignore_case=ignore_case,
                        on_change = on_change,report_change=report_change)
        self._init()

    def _init(self):
        self.pin.init()

    def _trigger_next_turn(self):
        self.turn_start=time.ticks_ms()
        self.write_angle(self.angle_list[0])
        del self.angle_list[0]

    def turn(self,msg):
        if type(msg) is str:
            self.angle_list=[int(msg)] # TODO: accept floats?
        else: # should be a list
            self.angle_list = msg
        self._trigger_next_turn()

    def write_us(self, us):
        """Set the signal to be ``us`` microseconds long. Zero disables it."""
        if us == 0:
            self.pin.duty(0)
            return
        us = min(self.max_us, max(self.min_us, us))
        duty = us * 1024 * self.freq // 1000000
        self.pin.duty(duty)

    def write_angle(self, degrees=None):
        degrees = degrees % 360
        total_range = self.max_us - self.min_us
        us = self.min_us + total_range * degrees // self.angle
        self.write_us(us)

    def _update(self):
        if self.turn_start is not None: # turn in process
            current=time.ticks_ms()
            if time.ticks_diff(current, self.turn_start) >= self.turn_time_ms:
                if len(self.angle_list) > 0:
                    self._trigger_next_turn()
                else:
                    self._release()

    def _release(self):
        self.turn_start = None
        self.pin.deinit()

    def value(self):
        return self.angle_list
