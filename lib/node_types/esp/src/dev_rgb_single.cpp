// dev_rgb_sigle.cpp
// control a multicolor led

#include "dev_rgb_single.h"
#include <_PWM.h> // analog write seems to be finally completely broken on the esp8266, so use our own

void RGB_Single::process_color(int lednr, CRGB color, bool show) {
    if(!started()) return;
    // ignore show
    // this is a controller for single led, so we ignore lednr
    if(invert) {
        // analogWrite(p_r, (255-color.red)*1023/255);
        // analogWrite(p_g, (255-color.green)*1023/255);
        // analogWrite(p_b, (255-color.blue)*1023/255);
        analogWrite(p_r, 255-color.red);
        analogWrite(p_g, 255-color.green);
        analogWrite(p_b, 255-color.blue);
    } else {
        // analogWrite(p_r, color.red*1023/255);
        // analogWrite(p_g, color.green*1023/255);
        // analogWrite(p_b, color.blue*1023/255);
        analogWrite(p_r, color.red);
        analogWrite(p_g, color.green);
        analogWrite(p_b, color.blue);
    }
    avg_color = color;
}