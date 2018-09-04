// rgb_sigle.cpp
// control a multicolor led

#include "rgb_single.h"

void RGB_Single::process_color(int lednr, CRGB color, bool show) {
    if(!started()) return;
    // ignore show
    // this is a controller for single led, so we ignore lednr
    if(invert) {
        analogWrite(p_r, (255-color.red)*1023/255);
        analogWrite(p_g, (255-color.green)*1023/255);
        analogWrite(p_b, (255-color.blue)*1023/255);
    } else {
        analogWrite(p_r, color.red*1023/255);
        analogWrite(p_g, color.green*1023/255);
        analogWrite(p_b, color.blue*1023/255);
    }
    avg_color = color;
}