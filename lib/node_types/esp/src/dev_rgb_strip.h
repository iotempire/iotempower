// rgb_strip.h
// control a multicolor led strip
// DEPRECATED - use rgb_strip_bus instead

#ifndef _RGB_STRIP_H_
#define _RGB_STRIP_H_

#include <FastLED.h>
#include "dev_rgb_base.h"

#define ALL_LEDS -16

// Deprecated RGB_Strip class using custom CRGB compatibility layer
class RGB_Strip : public RGB_Base {
    protected:
        CRGB *leds;
        CLEDController *controller;
    public:
        RGB_Strip(const char* name, int _led_count, CLEDController& _controller) 
        : RGB_Base(name, _led_count) {
            controller = &_controller;
            set_pollrate_us(10000); // give update chance - TODO: check if necessary
            leds = controller->leds();
        }
        
        // Override virtual methods from RGB_Base
        virtual void start() {
            _started = true;
            controller->init(); // re-init, might be important if using onboard-led
            delay(50); // let things settle
            set_color(ALL_LEDS, RGB_Color::Black);
            delay(100); // let things settle (total 100 is too small)
            set_color(ALL_LEDS, RGB_Color::Black);
        }
        
        // Conversion between RGB_Color and CRGB
        static CRGB toCRGB(const RGB_Color& c) {
            return CRGB(c.r, c.g, c.b);
        }
        static RGB_Color fromCRGB(const CRGB& c) {
            return RGB_Color(c.r, c.g, c.b);
        }

        virtual void process_color(int lednr, RGB_Color color, bool _show=true) {
            leds[lednr] = toCRGB(color);
            if(_show) show();
        }
        
        virtual RGB_Color get_color(int lednr) {
            if(lednr < 0) lednr=0;
            else if(lednr >= led_count()) lednr = led_count()-1;
            return fromCRGB(leds[lednr]);
        }
        
        virtual void show() {
            if(!started()) return;
            controller->showLeds(255); // TODO: handle brightness of showLeds?
            // TODO: consider only computing average every 10ms
            // compute average color
            uint32_t avg_r = 0;
            uint32_t avg_g = 0;
            uint32_t avg_b = 0;
            int lc = led_count();
            for(int i=lc-1; i>=0; i--) {
                avg_r += leds[i].r;
                avg_g += leds[i].g;
                avg_b += leds[i].b;
            }
            avg_color = RGB_Color(avg_r/lc, avg_g/lc, avg_b/lc);
        }

        virtual bool measure() { show(); return true; } // give control to library on regular basis
};

#endif // _RGB_STRIP_H_
