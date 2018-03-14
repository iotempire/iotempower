# Rgb driver for either led strips based on
# the ws2812b (also sold as neopixels) and respective strips
#
# author: ulno
# created: 2017-05-22
#

import gc
import neopixel

gc.collect()
from uiot._rgb_base import RGB_Base
from uiot import colors as _c

gc.collect()


class RGB_Multi(RGB_Base):
    def __init__(self, name, pin, num_of_leds=1,
                 ignore_case=True, on_change=None, rgb_order=(1, 2, 3),
                 report_change=False):
        self.rgb_order = rgb_order
        if num_of_leds <= 0: num_of_leds = 1
        self.ws_leds = num_of_leds
        pin = neopixel.NeoPixel(pin, self.ws_leds)
        RGB_Base.__init__(self, name, pin,
                          ignore_case=ignore_case,
                          on_change=on_change,
                          report_change=report_change)

    def get(self, led_num=None):
        if led_num is None:
            return self.current_rgb
        else:
            if led_num <= 0: led_num = 0
            return self.port[led_num]

    def _set(self, r, g, b, led_num=None):
        rgb = (r, g, b)
        r = rgb[self.rgb_order[0] - 1]
        g = rgb[self.rgb_order[1] - 1]
        b = rgb[self.rgb_order[2] - 1]
        if led_num is None:
            for led in range(self.ws_leds):
                self.port[led] = (r, g, b)
            self.current_rgb = (r, g, b)
            self.brightness = int((r + g + b) / 3)
        else:
            if led_num <= 0: led_num = 0
            self.port[led_num] = (r, g, b)
            # recompute average
            ar, ag, ab = _c.black
            brightness = 0
            for r, g, b in self.port:
                ar += r
                ag += g
                ab += b
                brightness += r + g + b
            self.current_rgb = (int(ar / self.ws_leds),
                                int(ag / self.ws_leds),
                                int(ab / self.ws_leds))
            self.brightness = int(brightness / self.ws_leds / 3)

    def _write(self):
        self.port.write()
