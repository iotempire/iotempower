// rgb_strip_bus.h
// control a multicolor led strip

#ifndef _RGB_STRIP_BUS_H_
#define _RGB_STRIP_BUS_H_

#include "dev_rgb_base.h"
#include <NeoPixelBus.h>

class RGB_Strip_Bus_Base {
    protected:
        bool _initialized=false;
    public:

        // Conversion between RGB_Color and RgbColor
        static RgbColor toRgbColor(const RGB_Color& c) {
            return RgbColor(c.r, c.g, c.b);
        }
        static RGB_Color fromRgbColor(const RgbColor& c) {
            return RGB_Color(c.R, c.G, c.B);
        }

        virtual void set_pixel(uint16_t indexPixel, RGB_Color color) {
            // Convert to RgbColor for NeoPixelBus
        }
        virtual RGB_Color get_pixel(uint16_t indexPixel) {
            // Convert from RgbColor for NeoPixelBus
            return RGB_Color::Black;
        }
        virtual void show() {
        }
        virtual void begin() {
        }
        bool initialized() {
            return _initialized;
        }

};

template<typename T_COLOR_FEATURE, typename T_METHOD> class RGB_Strip_Bus_Template: public RGB_Strip_Bus_Base {
    private:
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller;
    public:
        RGB_Strip_Bus_Template(uint16_t countPixels, uint8_t pin) {
            controller = new NeoPixelBus<T_COLOR_FEATURE, T_METHOD> (countPixels, pin);
            if(controller) _initialized = true;
        }
        virtual void set_pixel(uint16_t indexPixel, RGB_Color color) {
            controller->SetPixelColor(indexPixel, toRgbColor(color));
        }
        virtual RGB_Color get_pixel(uint16_t indexPixel) {
            return fromRgbColor(controller->GetPixelColor(indexPixel));
        }
        virtual void show() {
            controller->Show();
        }
        virtual void begin() {
            controller->Begin();
        }
};

class RGB_Strip_Bus : public RGB_Base {
    private:
//        CRGB *leds;
//        CLEDController *controller;
        RGB_Strip_Bus_Base *controller;
    public:
        RGB_Strip_Bus(const char* name, int _led_count, RGB_Strip_Bus_Base& _controller) 
        : RGB_Base(name, _led_count) {
            controller = &_controller;
            // set_pollrate_us(10); // seems not necessary
        }
        void start() {
            if(controller->initialized()) {
                _started = true;
                //controller->init(); // re-init, might be important if using onboard-led
                controller->begin();
                delay(50); // let things settle
                set_color(ALL_LEDS, RGB_Color::Black);
                delay(100); // let things settle (total 100 is too small)
                set_color(ALL_LEDS, RGB_Color::Black);
            } else {
                ulog("rgb strip not initialized."); // TODO: show device name
            }
        }
        virtual void process_color(int lednr, RGB_Color color, bool _show=true) {
            controller->set_pixel(lednr, color);
            if(_show) show();
        }
        virtual RGB_Color get_color(int lednr) {
            if(lednr < 0) lednr=0;
            else if(lednr >= led_count()) lednr = led_count()-1;
            return controller->get_pixel(lednr);
        }
        virtual void show() {
            if(!started()) return;
            controller->show(); // TODO: handle brightness of showLeds?
            // TODO: consider only computing average every 10ms
            // compute average color
            uint32_t avg_r = 0;
            uint32_t avg_g = 0;
            uint32_t avg_b = 0;
            int lc = led_count();
            for(int i=lc-1; i>=0; i--) {
                RGB_Color c = controller->get_pixel(i);
                avg_r += c.r;
                avg_g += c.g;
                avg_b += c.b;
            }
            avg_color = RGB_Color(avg_r/lc, avg_g/lc, avg_b/lc);
        }
};

#endif // _RGB_STRIP_H_