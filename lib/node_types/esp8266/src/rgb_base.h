// rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_

#include <FastLED.h>
#include <device.h>

class RGB_Base : public Device {
    private:
        CRGB avg_color;
    public:
        RGB_Base(const char* name);
        void high() { 
            set_color(CRGB::White);
        }
        void on() { high(); }
        void low() { 
            set_color(CRGB::Black);
        }
        void off() { low(); }
        void set_color(uint8_t r, uint8_t g, uint8_t b) {
            set_color(-1, CRGB(r,g,b));
        }
        void set_color(CRGB color) {
            set_color(-1, color);
        }
        virtual void set_color(int lednr, CRGB color) {
            avg_color = color;
        }
};


#endif // _RGB_BASE_H_