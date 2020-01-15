// analog.cc
#include "analog.h"

static unsigned long last_read = millis()-5;

bool Analog::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 3) return false;
    last_read = current_time;
    if(started()) { 
        int v = analogRead(A0);
        value().from(v);
        return true;
    }
    return false; // not started
}