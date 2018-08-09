// rgb_multi.h
// control a multicolor led strip

#ifndef _RGB_STRIP_H_
#define _RGB_STRIP_H_

#include <rgb_base.h>
// included in the former
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#include <FastLED.h>


class RGB_Strip : public RGB_Base {
    private:
        CRGB *leds;
        CLEDController *controller;
    public:
        RGB_Strip(const char* name, int _led_count) : RGB_Base(name, _led_count) {
        }
        RGB_Strip& init(CLEDController& _controller) {
            controller = &_controller;
            leds = controller->leds();
            set_color(ALL_LEDS, CRGB::Black);
            return *this;
        }
        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            if(lednr<0) {
                if(lednr == ALL_LEDS) {
                }
            } else {
                if(lednr < led_count()) leds[lednr] = color;
                if(_show) show();
            } 
        }
        virtual CRGB get_color(int lednr) {
            if(lednr < 0) lednr=0;
            else if(lednr >= led_count()) lednr = led_count()-1;
            return leds[lednr];
        }
        virtual void show() {
            controller->showLeds(); // TODO: handle brightness of showLeds?
        }
};

// weird hack to allow to use this wicked fastled template stuff to add the
// controller later and still do all initialization correctly
#define CREATE_RGB_STRIP(name, led_count, type, ...) \
    ((new RGB_Strip(name, led_count))\
        ->init(FastLED.addLeds< type, __VA_ARGS__ >\
            ((CRGB*) malloc(sizeof(CRGB) * led_count), led_count)))

#endif // _RGB_STRIP_H_