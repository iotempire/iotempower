// rgb_multi.h
// control a multicolor led strip

#ifndef _RGB_MULTI_H_
#define _RGB_MULTI_H_

#include <rgb_base.h>


template<uint8_t PIN, int LED_COUNT> class RGB_Multi : public RGB_Base {
    private:
        CRGB leds[LED_COUNT];
        const uint8_t _pin=PIN;
    public:
        RGB_Multi(const char* name) : RGB_Base(name, LED_COUNT) {
            pinMode(PIN, OUTPUT);
            //FastLED.addLeds<NEOPIXEL,PIN>(leds, LED_COUNT);
            FastLED.addLeds<WS2811,PIN,BRG>(leds, LED_COUNT);
            set_color(ALL_LEDS, CRGB::Black);
        }
        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            if(lednr<0) {
                if(lednr == ALL_LEDS) {
                }
            } else {
                if(lednr < LED_COUNT) leds[lednr] = color;
                if(_show) show();
            } 
        }
        virtual CRGB get_color(int lednr) {
            if(lednr < 0) lednr=0;
            else if(lednr >= LED_COUNT) lednr = LED_COUNT-1;
            return leds[lednr];
        }
        virtual void show() {
            FastLED.show();
        }
};

#endif // _RGB_MULTI_H_