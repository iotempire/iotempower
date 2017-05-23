# Rgb driver for either led's with three control pins
# or the ws2812b (also sold as neopixels) based chips and strips
#
# author: ulno
# created: 2017-05-22
#

import gc
import time
from machine import Pin
from ulnoiot.device import Device
from ulnoiot import colors as _c
gc.collect()

####### RGB leds or led strips
class RGB(Device):

    # Handle output devices
    def __init__(self, name, pin, *args,
                 ignore_case=True, on_change=None, rgb_order=(1, 2, 3)):
        self.ws_leds=0
        self.is_on=False
        self.last_rgb=_c.white
        self.current_rgb=_c.black
        self.rgb_order=rgb_order
        if len(args) <= 1: # either one rgb led or a number based on ws2812b
            self.ws_leds=1
            if len(args)==1:
                self.ws_leds=args[0]
            import neopixel
            pin=neopixel.NeoPixel(pin,self.ws_leds)
        else: # more given -> should be other pins
            from machine import PWM
            pin = (PWM(pin),PWM(args[0]),PWM(args[1]))
        Device.__init__(self, name, pin, *args,
                        setters={"brightness/set":self._set_brightness,
                                 "rgb/set":self._set_rgb,
                                 "set":self._set,
                                 "animation":self.set},
                        getters={"":self.get_status,
                                 "brightness/status":self.get_brightness,
                                 "rgb/status":self.get_rgb},
                        ignore_case=ignore_case,
                        on_change = on_change)

        self.set(*_c.black) # make sure it's off

    def set(self,r,g,b,led_num=None):
        rgb=(r,g,b)
        r=rgb[self.rgb_order[0] - 1]
        g=rgb[self.rgb_order[1] - 1]
        b=rgb[self.rgb_order[2] - 1]
        if self.ws_leds > 0:  # this is a programmable strip
            if led_num is None:
                for led in range(self.ws_leds):
                    self.pin[led] = (r,g,b)
                self.current_rgb = (r,g,b)
                self.brightness = int((r+g+b)/3)
            else:
                if led_num<=0: led_num=1
                self.pin[led_num-1] = (r,g,b)
                # recompute average
                ar,ag,ab = _c.black
                brightness=0
                for r,g,b in self.pin:
                    ar+=r
                    ag+=g
                    ab+=b
                    brightness+=r+g+b
                self.current_rgb = (int(ar/self.ws_leds),
                                    int(ag/self.ws_leds),
                                    int(ab/self.ws_leds))
                self.brightness = int(brightness/self.ws_leds/3)
            self.pin.write() # TODO: consider moving this to update
        else:
            pr,pg,pb = self.pin
            pr.duty(int(r*1023/255))
            pg.duty(int(g*1023/255))
            pb.duty(int(b*1023/255))
            self.current_rgb=(r,g,b)
            self.brightness = int((r + g + b) / 3)

    def _set(self,msg):
        rgb=None
        if msg == "on":
            if not self.is_on:
                if self.last_rgb==_c.black:
                    self.last_rgb=_c.white
                rgb=self.last_rgb
            else: # go to full brightness
                rgb=_c.white
            self.is_on = True
        elif msg == "off":
            if self.is_on:
                self.last_rgb = self.current_rgb
            rgb=_c.black
            self.is_on = False
        if rgb is not None:
            self.set(*rgb)

    def _split(self,s):
        args=[]
        for vs in s.split(','):
            args.extend(vs.split())
        return args

    def get_color(self,s):
        if s in _c.clist:
            return _c.clist[s]
        # it might be given in hex
        if len(s)==6:
            r=int("0x"+s[0:2])
            g=int("0x"+s[2:4])
            b=int("0x"+s[4:6])
            return(r,g,b)
        return None

    def _set_rgb(self,msg):
        args = self._split(msg)
        if len(args) == 3: # r,g,b
            self.set(int(args[0]),int(args[1]),int(args[2]))
        elif len(args) == 4: # num,r,g,b
            self.set(int(args[1]),int(args[2]),int(args[3]),led_num=int(args[0]))
        elif len(args) == 1: # color (or hex)
            self.set(*self.get_color(args[0]))
        elif len(args) == 2: # color (or hex)
            self.set(*self.get_color(args[1]),led_num=int(args[1]))
        self.last_rgb = self.current_rgb

    def _set_brightness(self,msg):
        b=int(msg)
        self.set(b,b,b)
        self.last_rgb = self.current_rgb

    def get_status(self):
        if self.is_on:
            return 'on'
        else:
            return 'off'

    def get_brightness(self):
        return self.brightness

    def get_rgb(self):
        return self.current_rgb

    def value(self):
        return "{},{},{}".format(self.get_status(),
                                 self.get_brightness(),self.get_rgb())
