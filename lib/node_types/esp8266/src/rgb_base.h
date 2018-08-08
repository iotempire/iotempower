// rgb_base.h
// rgb_base class (base for doing rgb-strips and leds)

#ifndef _RGB_BASE_H_
#define _RGB_BASE_H_

#define FASTLED_INTERRUPT_RETRY_COUNT 0
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

        virtual void process_color(int lednr, CRGB color, bool _show=true) {
            avg_color = color;
        }
        void push_front(CRGB color, bool _show=true);
        void push_back(CRGB color, bool _show=true);
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

enum Strip_Direction { Left_Down = 1, Left_Up, Right_Down, Right_Up, 
                    Down_Right, Down_Left, Up_Right, Up_Left};

typedef struct {
    int16_t stripnr;
    int16_t lednr;
} Strip_Address;

class Animator : public Device {
    private:
        Strip_Address* matrix;
        RGB_Base* strips[ULNOIOT_MAX_LED_STRIPS];
        int height = 0;
        int width = 0;
        int strip_count = 0;
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        #define ANIMATION_HANDLER std::function<void(Animator&)>
        ANIMATION_HANDLER _handler = NULL;
        #define ANIMATOR_COMMAND_HANDLER std::function<void(Animator&,const Ustring& command)>
        ANIMATOR_COMMAND_HANDLER _command_handler = NULL;
        Strip_Address* matrix_get( int x, int y ) {
            return matrix + width*y + x;
        }
        void matrix_set(int x, int y, int stripnr, int lednr) {
            if(x>=0 && y>=0 && x<width && y<height) {
                Strip_Address* pos = matrix_get(x,y);
                pos->stripnr = stripnr;
                pos->lednr = lednr;
            } else {
                log("Wrong position %d,%d for matrix requested.",x,y);
            }
        }
        void init(int w, int h) {
            width = w;
            height = h;
            matrix = (Strip_Address*)
                malloc(width*height*sizeof(Strip_Address)); // TODO: destructor?
            for(int y=0; y<height; y++)
                for(int x=0; x<width; x++)
                    matrix_set(x,y,-1,-1);
            add_subdevice(new Subdevice(""));
            add_subdevice(new Subdevice("set",true))->with_receive_cb(
                [&] (const Ustring& payload) {
                    measured_value(1).from(payload); // save it
                    if(_command_handler) _command_handler(*this, payload);
                    return true;
                }
            );
            set_fps(24);
        }
    public:
        Animator(const char* name, int _width, int _height) : Device(name) {
            init(_width,_height);            
        }
        Animator(const char* name, RGB_Base& strip) : Device(name) {
            init(strip.led_count(),1);
            add(strip, 0, 0);
        }
        Animator& add(RGB_Base& strip, int posx, int posy, 
            Strip_Direction direction=Right_Down, int linelen=-1);
        Animator& with(RGB_Base& strip, int posx, int posy, 
            Strip_Direction direction=Right_Down, int linelen=-1) {
            add(strip, posx, posy, direction, linelen);
            return *this;
        }
        Animator& with_animation_handler(ANIMATION_HANDLER handler) {
            _handler = handler;
            return *this;
        }
        Animator& with_command_handler(ANIMATOR_COMMAND_HANDLER handler) {
            _command_handler = handler;
            return *this;
        }
        int get_width() {
            return width;
        }
        int get_height() {
            return height;
        }
        void set_fps(int fps) {
            if(fps<1) fps=1;
            if(fps>100) fps=100;
            _fps = fps;
            frame_len = 1000 / fps;
        }
        Animator& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }
        void show() {
            for(int i=0; i<strip_count; i++)
                strips[i]->show();
        }
        bool measure();
        CRGB get_pixel(int x, int y, bool wrap=true) {
            Strip_Address *a;
            if(wrap) {
                x+=4*width;
                y+=4*height;
                a = matrix_get(x%width,y%height);
            } else {
                if(x<0 || y<0 || x>=width || y>=width) return CRGB(CRGB::Black);
                a = matrix_get(x,y);
            }
            if(a->stripnr >= 0) return strips[a->stripnr]->get_color(a->lednr);
            log("get_pixel, out of bounds: %d,%d",x,y); // TODO: remove debug
            return CRGB(CRGB::Black); // if something goes wrong, return black
        }
        CRGB set_pixel(int x, int y, CRGB color, bool wrap=true) {
            Strip_Address *a;
            if(wrap) {
                x+=4*width;
                y+=4*height;
                a = matrix_get(x%width,y%height);
            } else {
                if(x<0 || y<0 || x>=width || y>=width) return CRGB(CRGB::Black);
                a = matrix_get(x,y);
            }
            if(a->stripnr >= 0) {
                CRGB old = strips[a->stripnr]->get_color(a->lednr); 
                strips[a->stripnr]->set_color(a->lednr, color, false);
                return old;
            }
            log("set_pixel, out of bounds: %d,%d",x,y);  // TODO: remove debug
            return CRGB(CRGB::Black); // if something goes wrong, return black
        }
        // Matrix drawing functions
        void scroll_up(bool cycle=false, int column=-1);
        void scroll_down(bool cycle=false, int column=-1);
        void scroll_left(bool cycle=false, int line=-1);
        void scroll_right(bool cycle=false, int line=-1);
        // all off
        void black() {
            for(int y=0; y<height; y++) {
                for(int x=0; x<width; x++) {
                    set_pixel(x,y,CRGB::Black,false);
                }
            }
        }
        void off() { black(); }

        void rainbow(int startx=0, int starty=0,
                int w=-1, int h=-1,
                uint8_t initialhue=16, uint8_t deltahue=16 );

        void rainbow_row(int startx=0, int starty=0,
                int w=-1, int h=-1,
                uint8_t initialhue=16, uint8_t deltahue=16 );

        void rainbow_column(int startx=0, int starty=0,
                int w=-1, int h=-1,
                uint8_t initialhue=16, uint8_t deltahue=16 );

        void gradient_row( CRGB startcolor, CRGB endcolor,
                   int startx=0, int starty=0,
                   int w=-1, int h=-1 );

        void gradient_column( CRGB startcolor, CRGB endcolor,
                   int startx=0, int starty=0,
                   int w=-1, int h=-1 );

        void fade( uint8_t scale=4,
                int startx=0, int starty=0,
                int w=-1, int h=-1);
};


#endif // _RGB_BASE_H_