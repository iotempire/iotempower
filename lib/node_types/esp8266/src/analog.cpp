// input.cc
#include "analog.h"

static unsigned long last_read = millis()-5;

bool Analog::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 3) return false;
    last_read = current_time; 
//    noInterrupts();
    int v = analogRead(A0);
//    interrupts();
    if(measured_value().empty() 
            || abs(measured_value().as_int() - v) >= _precision ) {
        if(_threshold <= 0) {
            measured_value().from(v);
        } else {
            if(v > _threshold - _precision) {
                measured_value().from(_high);
            } else {
                measured_value().from(_low);
            }
        }
    }
    return true;
}