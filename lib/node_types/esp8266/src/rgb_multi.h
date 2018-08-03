// rgb_multi.h
// control a multicolor led strip

#ifndef _RGB_MULTI_H_
#define _RGB_MULTI_H_

#include <rgb_base.h>


template<uint8_t pin, int led_count> class RGB_Multi : public RGB_Base {
    private:
        CRGB leds[led_count];
        const int _led_count=led_count;
        const uint8_t _pin=pin;
    public:
        RGB_Multi(const char* name) : RGB_Base(name) {
            pinMode(pin, OUTPUT);
            FastLED.addLeds<NEOPIXEL,pin>(leds, led_count);
            set_color(-1, CRGB::Black);
        }
        virtual void set_color(int lednr, CRGB color) {
            if(lednr<0) {
                for(int nr=0; nr<_led_count; nr++) {
                    leds[nr] = color;
                }
            } else {
                if(lednr >= _led_count) lednr = _led_count - 1;
                leds[lednr] = color;
            }
            show();
        }
    private:
        void show() {
            FastLED.show();
        }
};

#endif // _RGB_MULTI_H_