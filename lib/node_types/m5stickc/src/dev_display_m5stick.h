// display.h
// Header file for display devices on the m5stickc

#ifndef _M5STICKC_DISPLAY_H_
#define _M5STICKC_DISPLAY_H_

#include <M5StickC.h>
#include <device.h>
#include <Text_Display_Buffer.h>

#define font_tiny 1
#define font_medium 2
#define font_large 3


class M5StickC_Display : public Device {
    private:
        Text_Display_Buffer* _tdb;
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        int _font = 1;
    protected:
        // allocate text buffer (return true if successful, false otherwise)
        bool init(int w, int h);
        int char_height, char_width;
    public:
        // this only supports the M5StickC displays so far
        M5StickC_Display(const char* name, int font=1) : Device(name) { // small font by default
            _tdb = new Text_Display_Buffer();
            _font = font;
            M5.begin();
            M5.Lcd.fillScreen(BLACK);
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
            return 160;
        }
        int height_pixels() {
            return 80;
        }

        virtual void show() {};
        
        bool measure();
};


#endif // _M5STICKC_DISPLAY_H