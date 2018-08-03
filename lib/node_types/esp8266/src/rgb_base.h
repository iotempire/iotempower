// rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_

#include <FastLED.h>
#include <device.h>

class RGB_Base : public Device {
    protected:
        int _led_count=0;
        CRGB avg_color;
    public:
        RGB_Base(const char* name, int led_count);
        void high() { 
            set_color(CRGB::White);
        }
        void on() { high(); }
        void low() { 
            set_color(CRGB::Black);
        }
        void off() { low(); }
        void set_color(CRGB color) {
            set_color(-1, color, true);
        }
        void set_color_noshow(CRGB color) {
            set_color(-1, color, false);
        }

        void set_colorstr(int lednr, const Ustring& color, bool _show=true);
        void set_colorstr(const Ustring& color, bool _show=true);

        virtual void set_color(int lednr, CRGB color, bool _show=true) {
            avg_color = color;
        }
        virtual CRGB get_color(int lednr) {
            return avg_color;
        }
        int led_count() {
            return _led_count;
        }
        virtual void show() {
            // nothing by default
        }

};


#endif // _RGB_BASE_H_