# Rgb base class for any type of
# rgb leds or led strips
#
# author: ulno
# created: 2017-05-22
#

import gc
from uiot.device import Device
from uiot import colors as _c

gc.collect()


class RGB_Base(Device):
    def __init__(self, name, port, ignore_case=True,
                 on_change=None, report_change=False):
        self.is_on = False
        self.last_rgb = _c.white
        self.current_rgb = _c.black
        self.ani = None
        b = "brightness"
        r = "rgb"
        s = "set"
        st = "/status"
        Device.__init__(self, name, port,
                        setters={b + "/" + s: self._command_brightness,
                                 r + "/" + s: self._command_rgb,
                                 s: self._command_status,
                                 "animation": self.animation},
                        getters={"": self.get_status,
                                 b + st: self.get_brightness,
                                 r + st: self.get},
                        ignore_case=ignore_case,
                        on_change=on_change,
                        report_change=report_change)
        self.set(*_c.black)  # off

    def _split(self, s):
        a = []
        for vs in s.split(','):
            a.extend(vs.split())
        return a

    def get_status(self):
        if self.is_on:
            return 'on'
        else:
            return 'off'

    def get_brightness(self):
        return self.brightness

    # get(self,led_num=None): override!

    # _set(self,r,g,b,led_num=None): override and sets one or all colors
    # (without activating it)

    # _write(self): needs to be overridden activates the set color(s)

    def set(self, r, g, b, led_num=None, no_write=False):
        self._set(r, g, b, led_num=led_num)
        if not no_write:
            self._write()  # TODO: to update?

    def _command_list_rgb(self, a, no_write=False):
        if len(a) == 3:  # r,g,b
            self.set(int(a[0]), int(a[1]), int(a[2]), no_write=no_write)
        elif len(a) == 4:  # num,r,g,b
            self.set(int(a[1]), int(a[2]), int(a[3]),
                     led_num=int(a[0]) - 1, no_write=no_write)
        elif len(a) == 1:  # color (or hex)
            self.set(*_c.get(a[0]), no_write=no_write)
        elif len(a) == 2:  # color (or hex)
            self.set(*_c.get(a[1]), led_num=int(a[0]) - 1,
                     no_write=no_write)
        self.last_rgb = self.current_rgb

    def _ani_stop(self):
        if self.ani is not None:
            self.ani.stop()

    def _command_rgb(self, msg):
        self._ani_stop()
        self._command_list_rgb(self._split(msg), no_write=False)

    def _command_brightness(self, msg):
        self._ani_stop()
        b = int(msg)
        self.set(b, b, b)
        self.last_rgb = self.current_rgb

    def _command_status(self, msg):
        self._ani_stop()
        rgb = None
        if msg == "on":
            if not self.is_on:
                if self.last_rgb == _c.black:
                    self.last_rgb = _c.white
                rgb = self.last_rgb
            else:  # full brightness
                rgb = _c.white
            self.is_on = True
        elif msg == "off":
            if self.is_on:
                self.last_rgb = self.current_rgb
            rgb = _c.black
            self.is_on = False
        if rgb is not None:
            self.set(*rgb)

    def measure(self):
        if self.ani is not None:
            self.ani.next()
        return self.get_status()

    def animation(self, msg):
        gc.collect()
        from uiot._rgb_animator import Animation
        gc.collect()
        self.ani = Animation(self, msg)

    def animation_is_playing(self):
        return self.ani is not None and self.ani.playing
