// rgb_base.cpp
// rgb_base class (base for doing rgb-strips and leds)

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


RGB_Base::RGB_Base(const char* name) :
    Device(name) {
    set_color(CRGB::White);
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
            set_color(brightness,brightness,brightness);
            return true;
        }
    );
    add_subdevice(new Subdevice("rgb"));
    add_subdevice(new Subdevice("rgb/set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            Ustring command(payload);
            int lednr = -1;
            command.strip();
            int p=command.find(" ");
            if(p>0) { // seems like these are two values
                // extract led number (start counting at 1)
                lednr = command.as_int();
                if(lednr < 1) lednr = 0;
                else lednr--;
                command.remove(0,p+1);
            }
            command.strip(); // remove more whitespace
            // Check if this is a color?
            unsigned int i;
            for(i=0; i<color_map_count; i++) {
                if(command.equals(color_map[i].key)) {
                    set_color(lednr, color_map[i].color);
                    return true;
                }
            }
            // we didn't get a match
            // check if it is a 6-byte hex value
            if(command.length() == 6) {
                char *endptr;
                long colorval = strtol(command.as_cstr(), &endptr, 16);
                if(endptr - command.as_cstr() == 6) {
                    set_color(lednr, CRGB(colorval));
                    return true;
                } 
            }
            // Last chance: see, if we can extract 3 comma seperated integers
            int r,g,b;
            if(sscanf(command.as_cstr(),"%d,%d,%d",&r,&b,&g) == 3) {
                set_color(lednr, CRGB(r,g,b));
            }
            return true;
        }
    );
    add_subdevice(new Subdevice("animation",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            return true;
        }
    );
}
