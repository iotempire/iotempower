// text_display_buffer.cpp
#include <text_display_buffer.h>

bool Text_Display_Buffer::init(int w, int h) {
    columns = w;
    lines = h;
    ulog(F("Display with %d columns and %d lines."), columns, lines);
    // create our own textbuffer
    textbuffer = (char *)malloc(lines*columns);
    if(!textbuffer) {
        ulog(F("Could not allocate textbuffer."));
        // TODO: anything which should be destructed now?
        return false;
    }
    return true;
}

Text_Display_Buffer& Text_Display_Buffer::scroll_up(int nr_lines) {
    // TODO: add cyclic scrolling
    char* from = textbuffer + nr_lines*columns;
    int block_h = lines-nr_lines;
    memmove(textbuffer, from, block_h*columns);
    memset(textbuffer + block_h*columns, ' ', nr_lines*columns);
    return *this;
}

Text_Display_Buffer& Text_Display_Buffer::print(const char* str) {
    // TODO: not capped by maxlen -> should not overflow because of Ustring given, but maybe better cap?
    char ch;
    while(*str) {
        ch = *str;
        switch(ch) {
            case '\n':
                cursor_y++;
                // no break to do next by default
            case '\r':
                cursor_x=0;
                ch=0;
                break;
            default:
                if(ch<' ') ch = ' ';
                break; 
        }
        if(ch!=0) {
            if(delayed_scroll) {
                scroll_up(1);
                delayed_scroll=false;
            }
            textbuffer[cursor_y * columns + cursor_x] = ch;
            changed = true;
            cursor_x ++;
            if(cursor_x >= columns) {
                cursor_x=0;
                cursor_y++;
            }
        }
        if(cursor_y >= lines) {
            // if(delayed_scroll) {
            //     scroll_up(1);
            // }
            delayed_scroll=true;
            cursor_y = lines-1;
        }
        str++;
    }
    return *this;
}

Text_Display_Buffer& Text_Display_Buffer::print(Ustring& ustr) {
    return print(ustr.as_cstr());
}

Text_Display_Buffer& Text_Display_Buffer::println() {
    return print(F("\n"));
}

Text_Display_Buffer& Text_Display_Buffer::println(const char* str) {
    print(str);
    return println();
}

Text_Display_Buffer& Text_Display_Buffer::println(Ustring& ustr) {
    return println(ustr.as_cstr());
}

Text_Display_Buffer& Text_Display_Buffer::cursor(int x, int y) {
    cursor_x = limit(x, 0, columns-1);
    cursor_y = limit(y, 0, lines-1);
    delayed_scroll = false;
    return *this;
}

Text_Display_Buffer& Text_Display_Buffer::clear() {
    memset(textbuffer, ' ', lines*columns);
    changed = true;
    cursor(0,0);
    delayed_scroll = false;
    return *this;
}
