# Rgb driver for rgb leds or rgb led strips for creating
# a single color via three control gpios
#
# author: ulno
# created: 2017-05-22
#

import gc
from machine import PWM
from uiot._rgb_base import RGB_Base

gc.collect()


class RGB(RGB_Base):
    def __init__(self, name, pinr, ping, pinb,
                 ignore_case=True, on_change=None,
                 report_change=False):
        pin = (PWM(pinr), PWM(ping), PWM(pinb))
        RGB_Base.__init__(self, name, pin,
                          ignore_case=ignore_case,
                          on_change=on_change,
                          report_change=report_change)

    def get(self, led_num=None):
        # led_num here ignored
        return self.current_rgb

    def _set(self, r, g, b, led_num=None):
        self.current_rgb = (r, g, b)
        self.brightness = int((r + g + b) / 3)

    def _write(self):
        (r, g, b) = self.current_rgb
        pr, pg, pb = self.pin
        m = 1023 / 255
        pr.duty(int(r * m))
        pg.duty(int(g * m))
        pb.duty(int(b * m))
