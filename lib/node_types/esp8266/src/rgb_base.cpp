// rgb_base.cpp
// rgb_base class (base for doing rgb-strips and leds)

#include <toolbox.h>
#include "rgb_base.h"

typedef struct {
    const char* key;
    CRGB color;
} String_Color;

static const String_Color color_map[] = {
    { "black", CRGB::Black },
    { "red", CRGB::Red },
    { "pink", CRGB::DeepPink },
    { "blue", CRGB::Blue },
    { "lightblue", CRGB::LightBlue },
    { "green", CRGB::Green },
    { "lightgreen", CRGB::LightGreen },
    { "yellow", CRGB::Yellow },
    { "magenta", CRGB::Magenta },
    { "cyan", CRGB::Cyan },
    { "purple", CRGB::Purple },
    { "orange", CRGB::Orange },
    { "brown", CRGB::Brown },
    { "gold", CRGB::Gold },
    { "grey", CRGB::Grey },
    { "lightgrey", CRGB::Grey },
    { "gray", CRGB::Grey },
    { "lightgray", CRGB::LightGrey },
    { "white", CRGB::White },
};
static const unsigned int color_map_count = sizeof(color_map)/sizeof(String_Color); 



void RGB_Base::push_front(CRGB color, bool _show) {
    for(int i=led_count()-1; i>0; i--) {
        set_color(i, get_color(i-1), false); // move colors up
    }
    set_color(0, color, _show);    
}

void RGB_Base::push_back(CRGB color, bool _show) {
    for(int i=0; i<led_count()-1; i++) {
        set_color(i, get_color(i+1), false); // move colors down
    }
    set_color(led_count()-1, color, _show);
}

RGB_Base::RGB_Base(const char* name, int led_count) :
        Device(name) {
    _led_count = led_count;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            if(payload.equals("on")) high();
            else {
                if(payload.equals("off")) low();
                else return false;
            }
            return true;
        }
    );
    add_subdevice(new Subdevice("brightness"));
    add_subdevice(new Subdevice("brightness/set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int brightness=payload.as_int();
            if(brightness<0) brightness=0;
            else if(brightness>255) brightness=255;
            set_color(CRGB(brightness,brightness,brightness));
            return true;
        }
    );
    add_subdevice(new Subdevice("rgb"));
    add_subdevice(new Subdevice("rgb/set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_colorstr(payload);
            return true;
        }
    );
    // add_subdevice(new Subdevice("animate",true))->with_receive_cb(
    //     [&] (const Ustring& payload) {
    //         Ustring command(payload);
    //         for(int i=0; i<(int)command_map_count; i++) {
    //             if(command.starts_with(command_map[i].key)) {
    //                 command.remove(0,strlen(command_map[i].key)+1);
    //                 command_map[i].method(*this, command);
    //                 break;
    //             }
    //         }
    //         return true;
    //     }
    // );
}

void RGB_Base::set_colorstr(int lednr, const Ustring& color, bool _show) {
    // Check if this is a color?
    unsigned int i;
    for(i=0; i<color_map_count; i++) {
        if(color.equals(color_map[i].key)) {
            set_color(lednr, color_map[i].color, _show);
            return;
        }
    }
    // we didn't get a match
    // check if it is a 6-byte hex value
    if(color.length() == 6) {
        char *endptr;
        long colorval = strtol(color.as_cstr(), &endptr, 16);
        if(endptr - color.as_cstr() == 6) {
            set_color(lednr, CRGB(colorval), _show);
            return;
        } 
    }
    // Last chance: see, if we can extract 3 comma seperated integers
    int r,g,b;
    if(sscanf(color.as_cstr(),"%d,%d,%d",&r,&b,&g) == 3) {
        set_color(lednr, CRGB(r,g,b), _show);
    }
}

void RGB_Base::set_colorstr(const Ustring& color, bool _show) {
    Ustring command(color);
    int lednr = ALL_LEDS;
    command.strip();
    int p=command.find(" ");
    if(p>0) { // seems like these are two values
        if(command.starts_with("front")) {
            lednr = -1;
        } else if(command.starts_with("back")) {
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
}
