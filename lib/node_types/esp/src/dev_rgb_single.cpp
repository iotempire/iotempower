// dev_rgb_sigle.cpp
// control a multicolor led

#include "dev_rgb_single.h"
#include <_PWM.h> // analog write seems to be finally completely broken on the esp8266, so use our own

void RGB_Single::process_color(int lednr, RgbColor color, bool show) {
    if(!started()) return;
    // ignore show
    // this is a controller for single led, so we ignore lednr
    if(invert) {
        // analogWrite(p_r, (255-color.R)*1023/255);
        // analogWrite(p_g, (255-color.G)*1023/255);
        // analogWrite(p_b, (255-color.B)*1023/255);
        analogWrite(p_r, 255-color.R);
        analogWrite(p_g, 255-color.G);
        analogWrite(p_b, 255-color.B);
    } else {
        // analogWrite(p_r, color.R*1023/255);
        // analogWrite(p_g, color.G*1023/255);
        // analogWrite(p_b, color.B*1023/255);
        analogWrite(p_r, color.R);
        analogWrite(p_g, color.G);
        analogWrite(p_b, color.B);
    }
    avg_color = color;
}