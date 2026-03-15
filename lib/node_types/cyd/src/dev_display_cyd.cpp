// display.cpp
#include <dev_display_cyd.h>

bool CYD_Display::init() {
    set_ignore_case(false); // be case sensitive

    _display->init();
    _display->setRotation(_rotation);
    _display->setTextSize(_text_size);
    set_color_bg(0,0,0);
    _display->fillScreen(_color_bg);
    set_color_fg(255,255,255);
    _display->setTextWrap(false);
    char_height = _display->fontHeight(_font)*_text_size;
    if(char_height<1) char_height=1;
    char_width = _display->textWidth("W", _font);
    if(char_width<1) char_width=1;
    set_fps(10); // can be low for these type of displays and just showing text
    int w = width_pixels() / char_width;
    if(w<1) w=1;
    int h = height_pixels() / char_height;
    if(h<1) h=1;
    if( !_tdb->init( w, h)) {
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
    if(_touchscreen) {
        _ts = new XPT2046_Touchscreen(XPT2046_CS, XPT2046_IRQ);
        _ts_spi = new SPIClass(VSPI);
        if(_ts && _ts_spi) {
            add_subdevice(new Subdevice(F("touch"),true));
            _ts_spi->begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
            _ts->begin(*_ts_spi);
            _ts->setRotation(_rotation);
        } else {
            _touchscreen = false;
            ulog(F("Failed to initialize touchscreen base objects."));
        }
    }
    return true;
}


bool CYD_Display::measure() {
    unsigned long current = millis();
    if(current - last_frame >= frame_len) {
        // only update when changed
        if(_tdb->get_changed()) {
            show();
            _tdb->reset_changed();
        }
        last_frame = current;
    }
    // TODO: think about pollrate
//    if(_touchscreen && _ts->tirqTouched() && _ts->touched()) {
    if(_touchscreen && _ts->tirqTouched()) {
        TS_Point p = _ts->getPoint();
        value(1).printf(F("%d,%d,%d"), p.x, p.y, p.z);
        return true;
    }
    return false;
}

void CYD_Display::show() {
    char charstr[2]=" ";
    int lines = get_lines();
    int columns = get_columns();
    const char* buffer = get_buffer();

    _display->fillScreen(_color_bg);

    for(int y=0; y<lines; y++) {
        for(int x=0; x<columns; x++) {
            charstr[0] = buffer[y * columns + x];
            _display->drawString(charstr, x*char_width, y*char_height + 1, _font );
        }
    }
}
