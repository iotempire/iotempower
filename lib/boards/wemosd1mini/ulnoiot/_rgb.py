# Rgb driver for either led's with three control pins
# or the ws2812b (also sold as neopixels) based chips and strips
#
# author: ulno
# created: 2017-05-22
#

import gc
from ulnoiot.device import Device
from ulnoiot import colors as _c
gc.collect()

class RGB(Device):
    def get_status(sf):
        if sf.is_on: return 'on'
        else: return 'off'

    def get_rgb(sf,led_num=None):
        if led_num is None or sf.ws_leds<=0:
            return sf.current_rgb
        else:
            if[led_num<=0]: led_num=1
            return sf.pin[led_num]
    # can't return brightness cause of mem overflow

    def __init__(sf, name, pin, *args,
                 ignore_case=True, on_change=None, rgb_order=(1,2,3)):
        sf.ws_leds=0
        sf.is_on=False
        sf.last_rgb=_c.white
        sf.current_rgb=_c.black
        sf.rgb_order=rgb_order
        sf.ani = None
        if len(args) <= 1: # either one rgb led or a number based on ws2812b
            sf.ws_leds=1
            if len(args)==1:
                sf.ws_leds=args[0]
            import neopixel
            gc.collect()
            pin=neopixel.NeoPixel(pin,sf.ws_leds)
        else: # more -> other pins
            from machine import PWM
            gc.collect()
            pin = (PWM(pin),PWM(args[0]),PWM(args[1]))
        b="brightness"; r="rgb"; s="set"; st="/status"
        Device.__init__(sf, name, pin,
                        setters={b+"/"+s:sf._set_brightness,
                                 r+"/"+s:sf._set_rgb,
                                 s:sf._set,
                                 "animation":sf.animation},
                        getters={"":sf.get_status,
                                 b+st:(lambda x:x.brightness),
                                 r+st:sf.get_rgb},
                        ignore_case=ignore_case,
                        on_change = on_change)
        sf.set(*_c.black) # off

    def _write_rgb(sf):
        if sf.ws_leds > 0:
            sf.pin.write()
        else:
            (r,g,b)=sf.current_rgb
            pr,pg,pb = sf.pin
            m=1023/255
            pr.duty(int(r*m))
            pg.duty(int(g*m))
            pb.duty(int(b*m))

    def set(sf,r,g,b,led_num=None,no_write=False):
        rgb=(r,g,b)
        r=rgb[sf.rgb_order[0] - 1]
        g=rgb[sf.rgb_order[1] - 1]
        b=rgb[sf.rgb_order[2] - 1]
        if sf.ws_leds > 0:  # programmable strip
            if led_num is None:
                for led in range(sf.ws_leds):
                    sf.pin[led] = (r,g,b)
                sf.current_rgb = (r,g,b)
                sf.brightness = int((r+g+b)/3)
            else:
                if led_num<=0: led_num=1
                sf.pin[led_num-1] = (r,g,b)
                # recompute average
                ar,ag,ab = _c.black
                brightness=0
                for r,g,b in sf.pin:
                    ar+=r
                    ag+=g
                    ab+=b
                    brightness+=r+g+b
                sf.current_rgb = (int(ar/sf.ws_leds),
                                    int(ag/sf.ws_leds),
                                    int(ab/sf.ws_leds))
                sf.brightness = int(brightness/sf.ws_leds/3)
        else:
            sf.current_rgb=(r,g,b)
            sf.brightness = int((r + g + b) / 3)
        if not no_write:
            sf._write_rgb() # TODO: to update?

    def _set(sf,msg):
        rgb=None
        if msg == "on":
            if not sf.is_on:
                if sf.last_rgb==_c.black:
                    sf.last_rgb=_c.white
                rgb=sf.last_rgb
            else: # full brightness
                rgb=_c.white
            sf.is_on = True
        elif msg == "off":
            if sf.is_on:
                sf.last_rgb = sf.current_rgb
            rgb=_c.black
            sf.is_on = False
        if rgb is not None:
            sf.set(*rgb)

    def _split(sf,s):
        a=[]
        for vs in s.split(','):
            a.extend(vs.split())
        return a

    def _set_rgb_list(sf,a,no_write=False):
        if len(a) == 3: # r,g,b
            sf.set(int(a[0]), int(a[1]), int(a[2]), no_write=no_write)
        elif len(a) == 4: # num,r,g,b
            sf.set(int(a[1]), int(a[2]), int(a[3]),
                     led_num=int(a[0]), no_write=no_write)
        elif len(a) == 1: # color (or hex)
            sf.set(*_c.get(a[0]), no_write=no_write)
        elif len(a) == 2: # color (or hex)
            sf.set(*_c.get(a[1]), led_num=int(a[0]),
                     no_write = no_write)
        sf.last_rgb = sf.current_rgb

    def _set_rgb(sf,msg,no_write=False):
        sf.ani.stop()
        sf._set_rgb_list(sf._split(msg),no_write=no_write)

    def _set_brightness(sf,msg):
        sf.ani.stop()
        b=int(msg)
        sf.set(b,b,b)
        sf.last_rgb = sf.current_rgb

    def _update(sf):
        if sf.ani is not None:
            sf.ani.next()

    def animation(sf,msg):
        gc.collect()
        from ulnoiot._rgb_animator import Animation
        gc.collect()
        sf.ani=Animation(msg)

    def value(sf):
        return (sf.get_status(),sf.brightness,sf.get_rgb())
