// dev_rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_


#include <device.h>
#include "rgb_color.h"

#define ALL_LEDS -16

class RGB_Base : public Device {
    protected:
        int _led_count=0;
    RGB_Color avg_color;
    public:
        RGB_Base(const char* name, int led_count);
        RGB_Base& high() {
            if((uint16_t)avg_color.r+avg_color.g+avg_color.b>0) {
                return *this; // if something is on, do nothing
            }
            return set_color(RGB_Color::White); // else switch to white TODO: make on-brightness configurable
        }
        RGB_Base& on() { return high(); }
        RGB_Base& low() {
            return set_color(RGB_Color::Black);
        }
        RGB_Base& off() { return low(); }
        RGB_Base& set_color(RGB_Color color) {
            return set_color(ALL_LEDS, color, true);
        }
        RGB_Base& set_color_noshow(RGB_Color color) {
            return set_color(ALL_LEDS, color, false);
        }

    bool read_color(const Ustring& colorstr, RGB_Color& color); // TODO: make static

    RGB_Base& set_colorstr(int lednr, const Ustring& color, bool _show=true);
    RGB_Base& set_colorstr(const Ustring& color, bool _show=true);

// TODO: implement setting a bar (percentage or number of leds at once)

    RGB_Base& set_color( int lednr, RGB_Color color, bool _show=true) {
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
    void push_front(RGB_Color color, bool _show=true);
    void push_back(RGB_Color color, bool _show=true);

        // these 4 need be re-implemented
        virtual void start() {
            // keep _started at false
        }

        virtual void process_color(int lednr, RGB_Color color, bool _show=true) {
            avg_color = color; // in strip, real average is computed
            // show not evaluated in single led case (but in rgb_strip)
        }

        virtual RGB_Color get_color(int lednr) {
            return avg_color;
        }

        virtual void show() {
            // nothing by default
        }

       virtual bool measure() { show(); return true; } // give control to library on regular basis
};

#endif // _RGB_BASE_H_