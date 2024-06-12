// display.h
// Header file for display devices on the m5stickc

#ifndef _M5STICKC_DISPLAY_H_
#define _M5STICKC_DISPLAY_H_

#include <platform_includes.h>
#include <device.h>
#include <text_display_buffer.h>

// using font sizes here 1-7
// #define font_tiny 1
// #define font_medium 2
// #define font_large 3


class M5StickC_Display : public Device {
    private:
        Text_Display_Buffer* _tdb;
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        u_int16_t _color_fg;
        u_int16_t _color_bg;
        int _font = 1;
        int _rotation = 0;
    protected:
        // allocate text buffer (return true if successful, false otherwise)
        bool init();
        int char_height, char_width;
    public:
        // this only supports the M5StickC displays so far
        M5StickC_Display(const char* name, int font=1, int rotation=0) : Device(name, 1000) { // small font by default
            _tdb = new Text_Display_Buffer();
            _font = font;
            if(rotation>3) {
                if(rotation==90) rotation = 1;
                else if(rotation==180) rotation = 2;
                else if(rotation==270) rotation = 3;
                else rotation = 0;
            }
            _rotation = rotation;
        }

        M5StickC_Display& scroll_up(int lines=1) {_tdb->scroll_up(lines); return *this;};
        M5StickC_Display& print(const char* str) {_tdb->print(str); return *this;};
        M5StickC_Display& print(Ustring& ustr) {_tdb->print(ustr); return *this;};
        M5StickC_Display& print(const __FlashStringHelper* str) {_tdb->print(str); return *this;}
        M5StickC_Display& println() {_tdb->println(); return *this;}
        M5StickC_Display& println(const char* str) {_tdb->println(str); return *this;};
        M5StickC_Display& println(Ustring& ustr) {_tdb->println(ustr); return *this;};
        M5StickC_Display& println(const __FlashStringHelper* str) {_tdb->println(str); return *this;}
        M5StickC_Display& cursor(int x, int y) {_tdb->cursor(x,y); return *this;}
        M5StickC_Display& clear() {_tdb->clear(); return *this;};

        void set_color_bg(u_int32_t c) {
            _color_bg = toRGB565(c);
        }

        void set_color_bg(u_int8_t r, u_int8_t g, u_int8_t b){
            set_color_bg((r<<16) + (g<<8) + b);
        }

        void set_color_fg(u_int32_t c) {
            _color_fg = toRGB565(c);
            M5.Lcd.setTextColor(_color_fg);
        }

        void set_color_fg(u_int8_t r, u_int8_t g, u_int8_t b) {
            set_color_fg((r<<16) + (g<<8) + b);
        }

        int get_lines() {return _tdb->get_lines();}
        int get_columns() {return _tdb->get_columns();}
        const char* get_buffer() {return _tdb->get_buffer();}

        void set_fps(int fps) {
            if(fps<1) fps=1;
            if(fps>100) fps=100;
            _fps = fps;
            frame_len = 1000 / fps;
        }
        M5StickC_Display& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }

        int width_pixels() {
            return M5.Lcd.width();
        }
        int height_pixels() {
            return M5.Lcd.height();
        }

        void show();
        
        void start() {
            if (init()) {
                _started = true;
            }
        }

        bool measure();
};


#endif // _M5STICKC_DISPLAY_H