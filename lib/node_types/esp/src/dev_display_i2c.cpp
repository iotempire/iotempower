// display.cpp
#include <dev_display_i2c.h>

bool Display_Base::init(int w, int h) {
    if( ! _tdb->init(w,h) ) {
        ulog(F("Failed to allocate display buffer."));
        return false;
    }
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


bool Display_Base::measure() {
    // only update when changed
    unsigned long current = millis();
    if(current - last_frame >= frame_len) {
        if(_tdb->get_changed()) {
            show();
            _tdb->reset_changed();
        }
        last_frame = current;
    }
    return false;
}


bool Display::init_u8g2() {
    _display->setFont(_font);
    char_height = _display->getMaxCharHeight();
    char_width = _display->getMaxCharWidth();
    set_fps(10); // can be low for these type of displays and just showing text
    if(_display->begin()) {
        if(init( _display->getWidth() / char_width, _display->getHeight() / char_height)) {
            clear();
        }
        return true;
    }
    return false;
}

void Display::show() {
    char charstr[2]=" ";
    int lines = get_lines();
    int columns = get_columns();
    const char* buffer = get_buffer();
    _display->firstPage();
    do {
        for(int y=0; y<lines; y++) {
            for(int x=0; x<columns; x++) {
                charstr[0] = buffer[y * columns + x];
                _display->drawStr(x*char_width, (y+1)*char_height-1, charstr);
            }
        }
    } while ( _display->nextPage() );
}

void Display_HD44780_I2C::init_hd44780_i2c(int w, int h, uint8_t scl, uint8_t sda, int i2c_addr) {
    set_address(i2c_addr);
    _width = w;
    _height = h;
    set_fps(2); // can be low for these type of displays and just showing text
}

void Display_HD44780_I2C::i2c_start() {
    _display = new LiquidCrystal_I2C(get_address(), _width, _height);
    if( ! _display ) {
        ulog("Can't reserve display memory, not starting display.");
        return;
    }
    _display->init(get_sda(), get_scl());
    if(init(_width, _height)) {
        clear();
        _display->cursor_off();
        _display->backlight();
    }
    _started = true;
}

void Display_HD44780_I2C::show() {
    char charstr[2]=" ";
    int lines = get_lines();
    int columns = get_columns();
    const char* buffer = get_buffer();
    for(int y=0; y<lines; y++) {
        _display->setCursor(0,y);
        for(int x=0; x<columns; x++) {
            charstr[0] = buffer[y * columns + x];
            _display->printstr(charstr);
        }
    }
}
