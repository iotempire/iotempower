// rgb_single.h
// control a one color rgb led or strip

#ifndef _RGB_SINGLE_H_
#define _RGB_SINGLE_H_

#include <rgb_base.h>

class RGB_Single : public RGB_Base {
    private:
        int p_r;
        int p_g;
        int p_b;
        bool invert;
        void init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert_signal) {
            p_r = pin_r;
            pinMode(p_r, OUTPUT);
            p_g = pin_g;
            pinMode(p_g, OUTPUT);
            p_b = pin_b;
            pinMode(p_b, OUTPUT);
            invert = invert_signal;
            // digitalWrite(p_r, invert);
            // digitalWrite(p_g, invert);
            // digitalWrite(p_b, invert);
            set_color(-1, CRGB::Black);
        }
    public:
        RGB_Single(const char* name, uint8_t pin_r, uint8_t pin_g, uint8_t pin_b) : RGB_Base(name) {
            init(pin_r, pin_g, pin_b, false);
        }
        RGB_Single(const char* name, uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert_signal) 
                : RGB_Base(name) {
            init(pin_r, pin_g, pin_b, invert_signal);
        }
        virtual void set_color(int lednr, CRGB color);
};


#endif // _RGB_BASE_H_