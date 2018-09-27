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
        RGB_Base& high() { 
            return set_color(CRGB::White);
        }
        RGB_Base& on() { return high(); }
        RGB_Base& low() { 
            return set_color(CRGB::Black);           
        }
        RGB_Base& off() { return low(); }
        RGB_Base& set_color(CRGB color) {
            return set_color(ALL_LEDS, color, true);
        }
        RGB_Base& set_color_noshow(CRGB color) {
            return set_color(ALL_LEDS, color, false);
        }

        RGB_Base& set_colorstr(int lednr, const Ustring& color, bool _show=true);
        RGB_Base& set_colorstr(const Ustring& color, bool _show=true);

// TODO: implement setting a bar (percentage or number of leds at once)

        RGB_Base& set_color( int lednr, CRGB color, bool _show=true) {
            if(!started()) return *this;
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
            return *this;
        }

        int led_count() {
            return _led_count;
        }
        void push_front(CRGB color, bool _show=true);
        void push_back(CRGB color, bool _show=true);

        // these 4 need be re-implemented
        virtual void start() {
            // keep _started at false
        }

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