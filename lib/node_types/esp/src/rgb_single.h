// rgb_single.h
// control a one color rgb led or strip

#ifndef _RGB_SINGLE_H_
#define _RGB_SINGLE_H_

#include <rgb_base.h>

#ifdef ESP32
    #include <analogWrite.h>
#endif

class RGB_Single : public RGB_Base {
    private:
        int p_r;
        int p_g;
        int p_b;
        bool invert;
        void init(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert_signal) {
            p_r = pin_r;
            p_g = pin_g;
            p_b = pin_b;
            invert = invert_signal;
        }
    public:
        RGB_Single(const char* name, uint8_t pin_r, uint8_t pin_g, uint8_t pin_b, bool invert_signal=false) 
                : RGB_Base(name, 1) {
            init(pin_r, pin_g, pin_b, invert_signal);
        }
        void start() {
            pinMode(p_r, OUTPUT);
            pinMode(p_g, OUTPUT);
            pinMode(p_b, OUTPUT);
            _started = true;
            set_color(ALL_LEDS, CRGB::Black);
        }
        virtual void process_color(int lednr, CRGB color, bool show=false);
        virtual CRGB get_color(int lednr) {
            return avg_color;
        }
        virtual void show() {
            // is all shown at set/process-time, so nothing here
        }
};


#endif // _RGB_BASE_H_