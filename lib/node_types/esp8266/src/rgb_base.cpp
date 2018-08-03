// rgb_base.cpp
// rgb_base class (base for doing rgb-strips and leds)

#include "rgb_base.h"
#include <toolbox.h>

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

void animate_cycle(RGB_Base& rgb_o, Ustring& command) {
}

void animate_pushfront(RGB_Base& rgb_o, Ustring& command) {
    for(int i=rgb_o.led_count(); i>0; i--) {
        rgb_o.set_color(i, rgb_o.get_color(i-1)); // move colors up
    }
    rgb_o.set_colorstr(0, command);    
}

void animate_pushback(RGB_Base& rgb_o, Ustring& command) {
    for(int i=0; i<rgb_o.led_count()-1; i++) {
        rgb_o.set_color(i, rgb_o.get_color(i+1)); // move colors up
    }
    rgb_o.set_colorstr(rgb_o.led_count()-1, command);
}

void animate_fade(RGB_Base& rgb_o, Ustring& command) {
    
}

void animate_run(RGB_Base& rgb_o, Ustring& command) {
    
}



typedef struct {
    const char* key;
    std::function<void (RGB_Base&, Ustring&)> method;
} Command_Method;

static const Command_Method command_map[] = {
    { "cycle", animate_cycle }, // create an animation cycle
    { "pushfront", animate_pushfront }, // push a color into front and cycle one directly
    { "pushback", animate_pushback }, // push a color into back and cycle one directly
    { "fade", animate_fade }, // define a fade goal
    { "run", animate_run } // start the actual animation
};
static const unsigned int command_map_count = sizeof(command_map)/sizeof(Command_Method); 


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
    add_subdevice(new Subdevice("animate",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            Ustring command(payload);
            for(int i=0; i<(int)command_map_count; i++) {
                if(command.starts_with(command_map[i].key)) {
                    command.remove(0,strlen(command_map[i].key)+1);
                    command_map[i].method(*this, command);
                    break;
                }
            }
            return true;
        }
    );
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
    int lednr = -1;
    command.strip();
    int p=command.find(" ");
    if(p>0) { // seems like these are two values
        // extract led number (start counting at 1)
        lednr = command.as_int();
        if(lednr < 1) lednr = 0;
        else lednr--;
        command.remove(0, p+1);
    }
    command.strip(); // remove more whitespace
    set_colorstr(lednr, command, _show);
}
