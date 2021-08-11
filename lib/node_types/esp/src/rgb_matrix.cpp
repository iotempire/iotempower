// rgb_matrix.cpp
// rgb_matrix class (base for doing rgb-strips and leds)

#include <toolbox.h>
#include "rgb_matrix.h"


RGB_Matrix& RGB_Matrix::add(RGB_Base& strip, int posx, int posy, 
        Strip_Direction direction, int linelen) {
    if(strip_count >= IOTEMPOWER_MAX_LED_STRIPS) {
        ulog(F("Too many led strips in animator."));
        return *this;
    }
    strips[strip_count] = &strip;
    int strip_nr = strip_count;
    strip_count ++;
    if( linelen <= 0) {
        linelen = strip.led_count();
        //// TODO: check - this here seems to cause something very weird
        //// not sure about this check
        //if(posy + linelen > height) {
        //    linelen = height - posy;
        //}
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
        ulog(F("Setting strip reference at %d,%d to %d,%d"),posx,posy,strip_nr,i);
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

void RGB_Matrix::scroll_up(bool cycle, int startx, int starty, int w, int h) {
    CRGB old;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    for(int x=startx; x<startx+w; x++) {
        if(cycle)
            old = get_pixel(x,starty);
        else
            old = CRGB::Black;
        for(int y=starty+h-1; y>=starty; y--) {
            old = set_pixel(x,y,old);
        }
    }
}

void RGB_Matrix::scroll_down(bool cycle, int startx, int starty, int w, int h) {
    CRGB old;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    for(int x=startx; x<startx+w; x++) {
        if(cycle)
            old = get_pixel(x,starty+h-1);
        else
            old = CRGB::Black;
        for(int y=starty; y<starty+h; y++) {
            old = set_pixel(x,y,old);
        }
    }
}

void RGB_Matrix::scroll_right(bool cycle, int startx, int starty, int w, int h) {
    CRGB old;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    for(int y=starty; y<starty+h; y++) {
        if(cycle)
            old = get_pixel(startx+w-1,y);
        else
            old = CRGB::Black;
        for(int x=startx; x<startx+w; x++) {
            old = set_pixel(x,y,old);
        }
    }
}

void RGB_Matrix::scroll_left(bool cycle, int startx, int starty, int w, int h) {
    CRGB old;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;

    for(int y=starty; y<starty+h; y++) {
        if(cycle)
            old = get_pixel(0,y);
        else
            old = CRGB::Black;
        for(int x=startx+w-1; x>=0; x--) {
            old = set_pixel(x,y,old);
        }
    }
}

void RGB_Matrix::rainbow( int startx, int starty, int w, int h,
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

void RGB_Matrix::rainbow_row( int startx, int starty,
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

void RGB_Matrix::rainbow_column( int startx, int starty,
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

void RGB_Matrix::gradient_row( CRGB startcolor, CRGB endcolor,
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

void RGB_Matrix::gradient_column( CRGB startcolor, CRGB endcolor,
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

void RGB_Matrix::fade_to(CRGB new_color, uint8_t scale,
        int startx, int starty,
        int w, int h) {
    CRGB color;
    if(w<0) w=width;
    if(h<0) h=height;
    if(startx+w >= width) w=width-startx;
    if(starty+h >= height) h=height-starty;
    for( int y = starty; y < starty + h; y++) {
        for( int x = startx; x < startx + w; x++) {
            color = get_pixel(x, y, false);
            color = blend(color, new_color, scale);
            set_pixel(x, y, color, false);
        }
    }
}

void RGB_Matrix::fade( uint8_t scale,
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

