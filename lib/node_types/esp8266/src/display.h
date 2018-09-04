// display.h
// Header file for display devices (supports all displays of U8g2-library
// https://github.com/olikraus/u8g2/)
// For displays and their initialization, check:
// https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#st7920-128x64
//
// To use a LCM1602 or HD44780 check other display driver at
// https://github.com/tonykambo/LiquidCrystal_I2C

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <U8g2lib.h> // most displays
#include <LiquidCrystal_I2C.h> // support for PCF8574T(i2C adapter) to Hitachi 44780/1602A/LCM1602
#include <SPI.h>
#include <Wire.h>
#include <device.h>

#define font_tiny u8g2_font_4x6_tr
#define font_medium u8g2_font_5x8_tr
#define font_large u8g2_font_8x13B_tr

class Display_Base : public Device {
    private:
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        char* textbuffer;
    protected:
        bool delayed_scroll=false;
        int cursor_x=0, cursor_y=0;
        int columns;
        int lines;
        // allocate text buffer (return true if successful, false otherwise)
        bool init(int w, int h);

        U8G2* _display;
        int char_height, char_width;
    public:
        Display_Base(const char* name) : Device(name) { }

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
        Display_Base& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }

        virtual void show(const char* buffer) {};
        
        bool measure();
};

static const char* display_ssd1306_failed = "u8g2 creation of ssd1306 failed.";

// This is based on the U82G displays
class Display : public Display_Base {
    private:
        void init_u8g2(U8G2& display, const uint8_t* font);
    public:
        // this only supports pixel-based (the ones with _1_) displays so far
        Display(const char* name, U8G2& display, 
            const uint8_t* font=font_medium) // small font by default
        : Display_Base(name) {
            init_u8g2(display, font);
        }
        Display(const char* name, const uint8_t* font=font_medium)
        : Display_Base(name) {
            // TODO: think to use templates to statically reserve space for display
            // TODO: do we need to specify default address 0x3c? 
            auto* d = new U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0);
            if(d) {
                init_u8g2(*d, font);
            } else {
                ulog(display_ssd1306_failed);
            }
        }
        Display(const char* name, uint8_t scl, uint8_t sda,
            const uint8_t* font=font_medium)
        : Display_Base(name) {
            // TODO: think to use templates to statically reserve space for display
            // TODO: do we need to specify default address 0x3c? 
            auto* d = new U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0, scl, sda);
            if(d) {
                init_u8g2(*d, font);
            } else {
                ulog(display_ssd1306_failed);
            }
        }

        u8g2_uint_t width_pixels() {
            return _display->getDisplayWidth();
        }
        u8g2_uint_t height_pixels() {
            return _display->getDisplayHeight();
        }

        virtual void show(const char* buffer);
};

class Display_HD44780_I2C : public Display_Base {
    private:
        LiquidCrystal_I2C* _display;
        void init_hd44780_i2c(int w, int h, uint8_t scl, uint8_t sda, int i2c_addr);
    public:
        Display_HD44780_I2C(const char* name, int w, int h, uint8_t scl, uint8_t sda, int i2c_addr=0x27) // or 0x38?
        : Display_Base(name) {
            init_hd44780_i2c(w, h, scl, sda, i2c_addr);
        }
        Display_HD44780_I2C(const char* name, int w, int h, int i2c_addr=0x27) // sometimes 0x38?
        : Display_Base(name) {
            init_hd44780_i2c(w, h, SCL, SDA, i2c_addr);
        }

        // TODO: check what happens when we use this together with the I2C hardware pins

        virtual void show(const char* buffer);
};

// TODO: implement 4bit (7outputs) control of HD44780 display (without I2C adapter)

#endif // _DISPLAY_H_