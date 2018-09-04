// rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_

// needs to be included here for color table
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_INTERNAL // ignore pragma messages in FastLED
#include <FastLED.h>

#include <device.h>

#define ALL_LEDS -16

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
            set_color(ALL_LEDS, color, true);
        }
        void set_color_noshow(CRGB color) {
            set_color(ALL_LEDS, color, false);
        }

        void set_colorstr(int lednr, const Ustring& color, bool _show=true);
        void set_colorstr(const Ustring& color, bool _show=true);

        void set_color( int lednr, CRGB color, bool _show=true) {
            if(lednr<0) {
                if(lednr==ALL_LEDS) {
                    for(int nr=0; nr<led_count(); nr++) 
                        process_color(nr, color, false);
                    if(_show) show();
                } else { // push front
                    push_front(color, _show);
                }
            } else if (lednr >= led_count()) {
                push_back(color, _show);
            } else {
                process_color(lednr, color, _show);
            }
        }

        int led_count() {
            return _led_count;
        }
        void push_front(CRGB color, bool _show=true);
        void push_back(CRGB color, bool _show=true);

        // these 3 should be re-implemented
        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            avg_color = color;
        }
        virtual CRGB get_color(int lednr) {
            return avg_color;
        }
        virtual void show() {
            // nothing by default
        }

};

#endif // _RGB_BASE_H_