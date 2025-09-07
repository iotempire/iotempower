// dev_rgb_base.cpp
// rgb_base class (base for doing rgb-strips and leds)

#include <toolbox.h>
#include "dev_rgb_base.h"

typedef struct {
    const char* key;
    ICRGB color;
} String_Color;

static const String_Color color_map[] = {
    { "black", ICRGB::Black },
    { "red", ICRGB::Red },
    { "pink", ICRGB::DeepPink },
    { "blue", ICRGB::Blue },
    { "lightblue", ICRGB::LightBlue },
    { "green", ICRGB(0, 255, 0) }, // fixed green
    { "lightgreen", ICRGB::LightGreen },
    { "yellow", ICRGB::Yellow },
    { "magenta", ICRGB::Magenta },
    { "cyan", ICRGB::Cyan },
    { "purple", ICRGB::Purple },
    { "orange", ICRGB::Orange },
    { "brown", ICRGB::Brown },
    { "gold", ICRGB::Gold },
    { "grey", ICRGB::Grey },
    { "lightgrey", ICRGB::Grey },
    { "gray", ICRGB::Grey },
    { "lightgray", ICRGB::LightGrey },
    { "white", ICRGB::White },
};
static const unsigned int color_map_count = sizeof(color_map)/sizeof(String_Color); 



void RGB_Base::push_front(ICRGB color, bool _show) {
    for(int i=led_count()-1; i>0; i--) {
        set_color(i, get_color(i-1), false); // move colors up
    }
    set_color(0, color, _show);    
}

void RGB_Base::push_back(ICRGB color, bool _show) {
    for(int i=0; i<led_count()-1; i++) {
        set_color(i, get_color(i+1), false); // move colors down
    }
    set_color(led_count()-1, color, _show);
}

RGB_Base::RGB_Base(const char* name, int led_count) :
        Device(name, 0) { // pollrate=0: might need to be super fast for matrix and animation
    _led_count = led_count;
    add_subdevice(new Subdevice()); // 0
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb( // 1
        [&] (const Ustring& payload) {
            if(payload.equals(str_on)) high();
            else {
                if(payload.equals(str_off)) low();
                else return false;
            }
            return true;
        }
    );
    add_subdevice(new Subdevice(F("brightness"))); // 2
    add_subdevice(new Subdevice(F("brightness/set"),true))->with_receive_cb( // 3
        [&] (const Ustring& payload) {
            int brightness=payload.as_int();
            if(brightness<0) brightness=0;
            else if(brightness>255) brightness=255;
            set_color(ICRGB(brightness,brightness,brightness));
            return true;
        }
    );
    add_subdevice(new Subdevice(F("rgb"))); // 4
    add_subdevice(new Subdevice(F("rgb/set"),true))->with_receive_cb( // 5
        [&] (const Ustring& payload) {
            set_colorstr(payload);
            return true;
        }
    );
}

bool RGB_Base::read_color(const Ustring& colorstr, ICRGB& color) {
    // Check if this is a predefined color, allow colorstr to have other values after color
    unsigned int i;
    for(i=0; i<color_map_count; i++) {
        if(colorstr.starts_with(color_map[i].key)) {
            if(colorstr.as_cstr()[strlen(color_map[i].key)] > ' ') return false; // needs to be terminated
            color = color_map[i].color;
            return true;
        }
    }
    // we didn't get a match
    // check if it is a 6-byte hex value
    if(colorstr.length() >= 6 && colorstr.as_cstr()[6] <= ' ') { // terminated by blank or none char
        char *endptr;
        long colorval = strtol(colorstr.as_cstr(), &endptr, 16);
        if(endptr - colorstr.as_cstr() == 6) {
            color = ICRGB((colorval >> 16) & 0xFF, (colorval >> 8) & 0xFF, colorval & 0xFF);
            return true;
        } 
    }
    // Last chance: see, if we can extract 3 comma seperated integers
    int r,g,b;
    if(sscanf(colorstr.as_cstr(),"%d,%d,%d",&r,&g,&b) == 3) {
        color = ICRGB(r,g,b);
        return true;
    }
    return false; // could not convert
}

RGB_Base& RGB_Base::set_colorstr(int lednr, const Ustring& color, bool _show) {
    ICRGB c;
    if(read_color(color, c)) {
        set_color(lednr, c, _show);
    } // TODO: should error be reported if no color found
    return *this;
}

RGB_Base& RGB_Base::set_colorstr(const Ustring& color, bool _show) {
    Ustring command(color);
    int lednr = ALL_LEDS;
    command.strip();
    int p=command.find(F(" "));
    if(p>0) { // seems like these are two values
        if(command.starts_with(F("front"))) {
            lednr = -1;
        } else if(command.starts_with(F("back"))) {
            lednr = led_count();
        } else {
            // extract led number (start counting at 1)
            lednr = command.as_int();
            if(lednr < 1) lednr = 0;
            else {
                lednr--;
                if(lednr>=led_count()) lednr=led_count()-1;
            }
        }
        command.remove(0, p+1);
    }
    command.strip(); // remove more whitespace
    set_colorstr(lednr, command, _show);
    return *this;
}
