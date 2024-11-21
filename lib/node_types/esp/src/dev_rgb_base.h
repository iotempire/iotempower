// dev_rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_

#define FASTLED_ESP8266_RAW_PIN_ORDER // solve issues with D4
//#define FASTLED_INTERRUPT_RETRY_COUNT 0 or better 1 or 2? - both seem to cause random flashes
#define FASTLED_INTERRUPT_RETRY_COUNT 2
#define FASTLED_INTERNAL // ignore pragma messages in FastLED
// needs to be included here for color table

// fastled uses an out function, so we need to undefine it first
#ifdef IOTEMPOWER_COMMAND_OUTPUT
#undef out
#endif
#include <FastLED.h>
#ifdef IOTEMPOWER_COMMAND_OUTPUT
#define out(gcc_va_args...) output(gcc_va_args)
#endif

#include <device.h>

#define ALL_LEDS -16

class RGB_Base : public Device {
    protected:
        int _led_count=0;
        CRGB avg_color;
    public:
        RGB_Base(const char* name, int led_count);
        RGB_Base& high() {
            if((uint16_t)avg_color.r+avg_color.g+avg_color.b>0) {
                return *this; // if something is on, do nothing
            }
            return set_color(CRGB::White); // else switch to white TODO: make on-brightness configurable
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

        bool read_color(const Ustring& colorstr, CRGB& color); // TODO: make static

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
            // update values with last color set
            // TODO: make this reporting skipable
            // TODO: seems like also on and off should be ignored for home-assistant
            value(0).from(avg_color.getLuma()>0?str_on:str_off);
            value(2).from((int)avg_color.getLuma());
            value(4).printf("%02x%02x%02x", avg_color.r, avg_color.g, avg_color.b);
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
            avg_color = color; // in strip, real average is computed
            // show not evaluated in single led case (but in rgb_strip)
        }

        virtual CRGB get_color(int lednr) {
            return avg_color;
        }

        virtual void show() {
            // nothing by default
        }

       virtual bool measure() { show(); return true; } // give control to library on regular basis
};

#endif // _RGB_BASE_H_