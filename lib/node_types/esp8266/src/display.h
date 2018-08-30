// display.h
// Header file for display devices (supports all displays of U8g2-library
// https://github.com/olikraus/u8g2/)
// For displays and their initialization, check:
// https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#st7920-128x64
//
// To use a LCM1602 or HD44780 check other display driver

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <device.h>

class Display : public Device {
    private:
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        U8G2* _display;
        char* textbuffer;
        bool delayed_newline=false;
        int cursor_x=0, cursor_y=0;
        int columns;
        int lines;
        int char_height, char_width;
        void init(U8G2& display, const uint8_t* font);
    public:
        // this only supports pixel-based displays so far
        Display(const char* name, U8G2& display, 
            const uint8_t* font=u8g2_font_4x6_mf) // small font by default
        : Device(name) {
            init(display, font);
        }
        u8g2_uint_t width() {
            return _display->getDisplayWidth();
        }
        u8g2_uint_t height() {
            return _display->getDisplayHeight();
        }
        void scroll_up(int lines=1);
        void print(const char* str);
        void println();
        void println(const char* str);
        void cursor(int x, int y);
        void clear();
        void set_fps(int fps) {
            if(fps<1) fps=1;
            if(fps>100) fps=100;
            _fps = fps;
            frame_len = 1000 / fps;
        }
        Display& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }
        
        virtual bool measure();
};


#endif // _DISPLAY_H_