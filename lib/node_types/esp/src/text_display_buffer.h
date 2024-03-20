// dev_display_base.h

#ifndef _TEXT_DISPLAY_BUFFER_H_
#define _TEXT_DISPLAY_BUFFER_H_

#include <device.h>

class Text_Display_Buffer {
    private:
        char* textbuffer;
        bool changed=true;
    protected:
        bool delayed_scroll=false;
        int cursor_x=0, cursor_y=0;
        int columns;
        int lines;

        int char_height, char_width;
    public:
        // allocate text buffer (return true if successful, false otherwise)
        bool init(int w, int h);
        Text_Display_Buffer() {} 
        Text_Display_Buffer(int w, int h) { 
            init(w, h);
        }

        Text_Display_Buffer& scroll_up(int lines=1);
        Text_Display_Buffer& print(const char* str);
        Text_Display_Buffer& print(Ustring& ustr);
        Text_Display_Buffer& print(const __FlashStringHelper* str) {
            Ustring str_u;
            str_u.from(str);
            print(str_u);
            return *this;
        }
        Text_Display_Buffer& println();
        Text_Display_Buffer& println(const char* str);
        Text_Display_Buffer& println(Ustring& ustr);
        Text_Display_Buffer& println(const __FlashStringHelper* str) {
            Ustring str_u;
            str_u.from(str);
            println(str_u);
            return *this;
        }
        Text_Display_Buffer& cursor(int x, int y);
        Text_Display_Buffer& clear();

        bool get_changed() { return changed; }
        void reset_changed() { changed = false; }
        int get_lines() { return lines; }
        int get_columns() { return columns; }

        const char *get_buffer() {
            return textbuffer;
        }        
};

#endif // _TEXT_DISPLAY_BUFFER_H_