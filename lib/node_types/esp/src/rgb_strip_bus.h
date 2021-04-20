// rgb_strip_bus.h
// control a multicolor led strip

#ifndef _RGB_STRIP_BUS_H_
#define _RGB_STRIP_BUS_H_

#include <rgb_base.h>
#include <NeoPixelBus.h>

class RGB_Strip_Bus_Base {
    protected:
        bool _initialized=false;
    public:

        virtual void set_pixel(uint16_t indexPixel, CRGB color) {
        }
        virtual CRGB get_pixel(uint16_t indexPixel) {
            return CRGB::Black;
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
        virtual void set_pixel(uint16_t indexPixel, CRGB color) {
            RgbColor new_color(color.red, color.green, color.blue);
            controller->SetPixelColor(indexPixel, new_color);
        }
        virtual CRGB get_pixel(uint16_t indexPixel) {
            RgbColor c = controller->GetPixelColor(indexPixel);
            return CRGB(c.R, c.G, c.B);
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
                set_color(ALL_LEDS, CRGB::Black);
                delay(100); // let things settle (total 100 is too small)
                set_color(ALL_LEDS, CRGB::Black);
            } else {
                ulog("rgb strip not initialized."); // TODO: show device name
            }
        }
        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            controller->set_pixel(lednr, color);
            if(_show) show();
        }
        virtual CRGB get_color(int lednr) {
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
            CRGB c;
            int lc = led_count();
            for(int i=lc-1; i>=0; i--) {
                c = controller->get_pixel(i);
                avg_r += c.r;
                avg_g += c.g;
                avg_b += c.b;
            }
            avg_color = CRGB(avg_r/lc, avg_g/lc, avg_b/lc);
        }
//        virtual bool measure() { show(); } // seems not necessary
};

#endif // _RGB_STRIP_H_