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
        RGB_Strip(const char* name, int _led_count, CLEDController& _controller) 
        : RGB_Base(name, _led_count) {
            controller = &_controller;
            leds = controller->leds();
        }
        void start() {
            _started = true;
            controller->init(); // re-init, might be important if using onboard-led
            delay(50); // let things settle
            set_color(ALL_LEDS, CRGB::Black);
            delay(100); // let things settle (total 100 is too small)
            set_color(ALL_LEDS, CRGB::Black);
        }
        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            leds[lednr] = color;
            if(_show) show();
        }
        virtual CRGB get_color(int lednr) {
            if(lednr < 0) lednr=0;
            else if(lednr >= led_count()) lednr = led_count()-1;
            return leds[lednr];
        }
        virtual void show() {
            if(!started()) return;
            controller->showLeds(); // TODO: handle brightness of showLeds?
        }
};

#endif // _RGB_STRIP_H_