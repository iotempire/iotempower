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



Animator& Animator::add(RGB_Base& strip, int posx, int posy, 
        Strip_Direction direction, int linelen) {
    if(strip_count >= ULNOIOT_MAX_LED_STRIPS) {
        log("Too many led strips in animator.");
        return *this;
    }
    strips[strip_count] = &strip;
    int strip_nr = strip_count;
    strip_count ++;
    if( linelen <= 0) {
        linelen = strip.led_count();
    }
    int xdir, ydir, xline, yline;;
    switch(direction) {
        case Right_Down:
            xdir = 1;
            ydir = 0;
            xline = 0;
            yline = 1;
            break;
        case Right_Up:
            xdir = 1;
            ydir = 0;
            xline = 0;
            yline = -1;
            break;
        case Left_Down:
            xdir = -1;
            ydir = 0;
            xline = 0;
            yline = 1;
            break;
        case Left_Up:
            xdir = -1;
            ydir = 0;
            xline = 0;
            yline = -1;
            break;
        case Down_Right:
            xdir = 0;
            ydir = 1;
            xline = 1;
            yline = 0;
            break;
        case Down_Left:
            xdir = 0;
            ydir = 1;
            xline = -1;
            yline = 0;
            break;
        case Up_Right:
            xdir = 0;
            ydir = -1;
            xline = 1;
            yline = 0;
            break;
        case Up_Left:
            xdir = 0;
            ydir = -1;
            xline = -1;
            yline = 0;
            break;
        default:
            // doesn't exist
            return *this;
    }
    for(int i=0; i<strip.led_count(); i++) {
        matrix_set(posx, posy, strip_nr, i);
        log("Setting strip reference at %d,%d to %d,%d",posx,posy,strip_nr,i); // TODO: remove debug
        if((i+1)%linelen == 0) { // at lineend
            posx += xline;
            posy += yline;
            // reverse direction
            xdir *= -1;
            ydir *= -1;
        } else {
            posx += xdir;
            posy += ydir;
        }
    }
    return *this;
}

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

// measure calls show in the end, so no show in graphics functions.
bool Animator::measure() {
    if(_handler) {
        unsigned long current = millis();
        int count=0;
        bool changed=false;
        while(current - last_frame >= frame_len) {
            _handler(*this);
            changed = true;
            last_frame += frame_len;
            count ++;
            if( count>=3 ) {
                last_frame = current;
                break; // only replay max 3 frames
            }
        }
        if(changed) show();
    }
    return false; // no values are generated here TODO: or true to return last command or frame number?
}


void Animator::scroll_up(bool cycle) {
    CRGB old;
    for(int x=0; x<width; x++) {
        if(cycle)
            old = get_pixel(x,0);
        else
            old = CRGB::Black;
        for(int y=height-1; y>=0; y--) {
            old = set_pixel(x,y,old);
        }
    }
}

void Animator::scroll_down(bool cycle) {
    CRGB old;
    for(int x=0; x<width; x++) {
        if(cycle)
            old = get_pixel(x,height-1);
        else
            old = CRGB::Black;
        for(int y=0; y<height; y++) {
            old = set_pixel(x,y,old);
        }
    }
}

void Animator::scroll_right(bool cycle) {
    CRGB old;
    for(int y=0; y<height; y++) {
        if(cycle)
            old = get_pixel(width-1,y);
        else
            old = CRGB::Black;
        for(int x=0; x<width; x++) {
            old = set_pixel(x,y,old);
        }
    }
}

void Animator::scroll_left(bool cycle) {
    CRGB old;
    for(int y=0; y<height; y++) {
        if(cycle)
            old = get_pixel(0,y);
        else
            old = CRGB::Black;
        for(int x=width-1; x>=0; x--) {
            old = set_pixel(x,y,old);
        }
    }
}

void Animator::rainbow( int startx, int starty,
            int w, int h,
            uint8_t initialhue, uint8_t deltahue ) {
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    CHSV hsv_rowstart, hsv;
    hsv_rowstart.hue = initialhue;
    hsv_rowstart.val = 255;
    hsv_rowstart.sat = 240;
    for( int x = startx; x < startx + w; x++) {
        hsv = hsv_rowstart;
        for( int y = starty; y < starty + h; y++) { 
            set_pixel(x, y, hsv, false);
            hsv.hue += deltahue;
        }
        hsv_rowstart.hue += deltahue;
    }

}

void Animator::rainbow_row( int startx, int starty,
            int w, int h,
            uint8_t initialhue,
            uint8_t deltahue )
{
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    for( int x = startx; x < startx + w; x++) {
        for( int y = starty; y < starty + h; y++) { 
            set_pixel(x, y, hsv, false);
        }
        hsv.hue += deltahue;
    }
}

void Animator::rainbow_column( int startx, int starty,
            int w, int h,
            uint8_t initialhue,
            uint8_t deltahue )
{
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;

    for( int y = starty; y < starty + h; y++) {
        for( int x = startx; x < startx + w; x++) { 
            set_pixel(x, y, hsv, false);
        }
        hsv.hue += deltahue;
    }
}

void Animator::gradient_row( CRGB startcolor, CRGB endcolor,
                   int startx, int starty,
                   int w, int h )
{
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    saccum87 rdistance87;
    saccum87 gdistance87;
    saccum87 bdistance87;

    rdistance87 = (endcolor.r - startcolor.r) << 7;
    gdistance87 = (endcolor.g - startcolor.g) << 7;
    bdistance87 = (endcolor.b - startcolor.b) << 7;

    uint16_t pixeldistance = w;
    int16_t divisor = pixeldistance ? pixeldistance : 1;

    saccum87 rdelta87 = rdistance87 / divisor;
    saccum87 gdelta87 = gdistance87 / divisor;
    saccum87 bdelta87 = bdistance87 / divisor;

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    accum88 r88 = startcolor.r << 8;
    accum88 g88 = startcolor.g << 8;
    accum88 b88 = startcolor.b << 8;
    for( int x = startx; x < startx + w; x++) {
        CRGB color( r88 >> 8, g88 >> 8, b88 >> 8);
        for( int y = starty; y < starty + h; y++) { 
            set_pixel(x, y, color, false);
        }
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

void Animator::gradient_column( CRGB startcolor, CRGB endcolor,
                   int startx, int starty,
                   int w, int h )
{
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    saccum87 rdistance87;
    saccum87 gdistance87;
    saccum87 bdistance87;

    rdistance87 = (endcolor.r - startcolor.r) << 7;
    gdistance87 = (endcolor.g - startcolor.g) << 7;
    bdistance87 = (endcolor.b - startcolor.b) << 7;

    uint16_t pixeldistance = w;
    int16_t divisor = pixeldistance ? pixeldistance : 1;

    saccum87 rdelta87 = rdistance87 / divisor;
    saccum87 gdelta87 = gdistance87 / divisor;
    saccum87 bdelta87 = bdistance87 / divisor;

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    accum88 r88 = startcolor.r << 8;
    accum88 g88 = startcolor.g << 8;
    accum88 b88 = startcolor.b << 8;
    for( int y = starty; y < starty + h; y++) {
        CRGB color( r88 >> 8, g88 >> 8, b88 >> 8);
        for( int x = startx; x < startx + w; x++) { 
            set_pixel(x, y, color, false);
        }
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

void Animator::fade( uint8_t scale,
        int startx, int starty,
        int w, int h) {
    CRGB color;
    scale = 255 - scale;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;
    for( int y = starty; y < starty + h; y++) {
        for( int x = startx; x < startx + w; x++) {
            color = get_pixel(x, y, false);
            color.nscale8(scale);
            set_pixel(x, y, color, false);
        }
    }
}

