// input.cc
#include "input.h"

bool Input::measure() {
    if(digitalRead(_pin)) {
        debouncer ++;
        if(debouncer > _threshold * 2 - 1)
            debouncer = _threshold * 2 - 1; 
    } else {
        debouncer --;
        if(debouncer < 0)
            debouncer = 0;
    }
    if( debouncer >= _threshold) {
        measured_value().from(_high);
    } else {
        measured_value().from(_low);
    }
    #ifdef mqtt_discovery_prefix
        create_discovery_info("binary_sensor", true, _high, _low, false, "");
    #endif
    return true;
}