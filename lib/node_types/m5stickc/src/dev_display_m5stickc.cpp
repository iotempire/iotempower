// display.cpp
#include <dev_display_m5stickc.h>

bool M5StickC_Display::init() {
    set_ignore_case(false); // be case sensitive

    M5.begin(true, true, false); // init all but serial
    // M5.Lcd.begin();
    // M5.Axp.begin();
    M5.Lcd.setRotation(_rotation);
    set_color_bg(0,0,0);
    M5.Lcd.fillScreen(_color_bg);
    set_color_fg(255,255,255);
    M5.Lcd.setTextSize(_font);
    M5.Lcd.setTextWrap(false);
    char_height = M5.Lcd.fontHeight();
    char_width = M5.Lcd.textWidth("W");
    set_fps(10); // can be low for these type of displays and just showing text
    if( !_tdb->init( width_pixels() / char_width, height_pixels() / char_height)) {
        ulog(F("Failed to allocate display buffer."));
        return false;
    }

    clear(); // empty text buffer

    add_subdevice(new Subdevice("",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            Ustring command(payload);
            Ustring subcommand;
            while(true) {
                int pos=command.find(F("&&"));
                if(pos<0) break;
                subcommand.copy(command, pos);
                print(subcommand.as_cstr());
                command.remove(pos+2);
                if(command.starts_with(F("nl")) || command.starts_with(F("ln"))) {
                    command.strip_param();
                    println();
                } else if(command.starts_with(F("cl"))) {
                    command.strip_param();
                    clear();
                    if(command.length()==0) return true; // skip newline at end
                } else if(command.starts_with(F("go"))) {
                    command.strip_param();
                    int x=command.as_int()-1;
                    command.strip_param();
                    int y=command.as_int()-1;
                    command.strip_param();
                    cursor(x,y);
                    if(command.length()==0) return true; // skip newline at end
                } else if(command.starts_with(F("fg"))) {
                    command.strip_param();
                    char *endptr;
                    long color = strtol(command.as_cstr(), &endptr, 16);
                    command.strip_param();
                    set_color_fg(color);
                    if(command.length()==0) return true; // skip newline at end
                } else if(command.starts_with(F("bg"))) {
                    command.strip_param();
                    char *endptr;
                    long color = strtol(command.as_cstr(), &endptr, 16);
                    command.strip_param();
                    set_color_bg(color);
                    if(command.length()==0) return true; // skip newline at end
                } else { // unknown
                    print(F("&&"));
                }
            }
            // Anything coming here should usually just be printed
            println(command.as_cstr());
            return true;
        }
    );
    return true;
}


bool M5StickC_Display::measure() {
    unsigned long current = millis();
    if(current - last_frame >= frame_len) {
        // only update when changed
        if(_tdb->get_changed()) {
            show();
            _tdb->reset_changed();
        }
        last_frame = current;
    }
    return false;
}

void M5StickC_Display::show() {
    char charstr[2]=" ";
    int lines = get_lines();
    int columns = get_columns();
    const char* buffer = get_buffer();

    M5.Lcd.fillScreen(_color_bg);

    for(int y=0; y<lines; y++) {
        for(int x=0; x<columns; x++) {
            charstr[0] = buffer[y * columns + x];
            M5.Lcd.drawString(charstr, x*char_width, y*char_height + 1 );
        }
    }
}
