// display.cpp
#include <display.h>

void Display::init(U8G2& display, const uint8_t* font) {
    set_ignore_case(false); // want to use capital letters
    _display = &display;
    _display->setFont(font); // TODO: check if this has to be reset in each frame?
    char_height = _display->getMaxCharHeight();
    char_width = _display->getMaxCharWidth();
    columns = _display->getWidth() / char_width;
    lines = _display->getHeight() / char_height;
    ulog("Display with %d columns and %d lines.", columns, lines);
    // create our own textbuffer
    textbuffer = (char *)malloc(lines*columns);
    if(!textbuffer) {
        ulog("Could not allocate textbuffer.");
        // TODO: anything which should be destructed now?
    } else {
        clear();
        add_subdevice(new Subdevice("",true))->with_receive_cb(
            [&] (const Ustring& payload) {
                Ustring command(payload);
                Ustring subcommand;
                while(true) {
                    int pos=command.find("&&");
                    if(pos<0) break;
                    subcommand.from_ustring(command, pos);
                    print(subcommand.as_cstr());
                    command.remove(pos+2);
                    if(command.starts_with("nl") || command.starts_with("ln")) {
                        command.strip_param();
                        println();
                    } else if(command.starts_with("cl")) {
                        command.strip_param();
                        clear();
                    } else if(command.starts_with("go")) {
                        command.strip_param();
                        int x=command.as_int()-1;
                        command.strip_param();
                        int y=command.as_int()-1;
                        command.strip_param();
                        cursor(x,y);
                    } else { // unknown
                        print("&&");
                    }
                }
                // Anything coming here should usually just be printed
                println(command.as_cstr());
                return true;
            }
        );
        _display->begin();
    }
}

void Display::scroll_up(int nr_lines) {
    // TODO: add cyclic scrolling
    char* from = textbuffer + nr_lines*columns;
    int block_h = lines-nr_lines;
    memmove(textbuffer, from, block_h*columns);
    memset(textbuffer + block_h*columns, ' ', nr_lines*columns);
}

void Display::print(const char* str) {
    // TODO: not capped by maxlen -> should not overflow because of Ustring given, but maybe better cap?
    char ch;
    while(*str) {
        ch = *str;
        switch(ch) {
            case '\n':
                cursor_y++;
            case '\r':
                cursor_x=0;
                ch=0;
                break;
            default:
                if(ch<' ') ch = ' ';
                break; 
        }
        if(ch!=0) {
            if(delayed_newline) {
                scroll_up(1);
                delayed_newline=false;
            }
            textbuffer[cursor_y * columns + cursor_x] = ch;
            cursor_x ++;
            if(cursor_x >= columns) {
                cursor_x=0;
                cursor_y++;
            }
        }
        if(cursor_y >= lines) {
            if(delayed_newline) {
                scroll_up(1);
            }
            delayed_newline=true;
            cursor_y = lines-1;
        }
        str++;
    }
}

void Display::println() {
    print("\n");
}

void Display::println(const char* str) {
    print(str);
    println();
}

void Display::cursor(int x, int y) {
    cursor_x = limit(x, 0, columns-1);
    cursor_y = limit(y, 0, lines-1);
    delayed_newline = false;
}

void Display::clear() {
    memset(textbuffer, ' ', lines*columns);
    cursor(0,0);
    delayed_newline = false;
}

bool Display::measure() {
    // TODO: restrict framerate
    char charstr[2]=" ";
    _display->firstPage();
    do {
        for(int y=0; y<lines; y++) {
            for(int x=0; x<columns; x++) {
                charstr[0] = textbuffer[y * columns + x];
                _display->drawStr(x*char_width, (y+1)*char_height-1, charstr);
            }
        }
    } while ( _display->nextPage() );
    return false;
}