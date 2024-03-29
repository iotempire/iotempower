// dev_input_digital.cpp
#include "dev_input_digital.h"

bool Input::measure() {
    if(read()) {
        debouncer ++;
        if(debouncer > _threshold * 2 - 1)
            debouncer = _threshold * 2 - 1; 
    } else {
        debouncer --;
        if(debouncer < 0)
            debouncer = 0;
    }
    if( debouncer >= _threshold) {
        value().from(_inverted?_low:_high);
    } else {
        value().from(_inverted?_high:_low);
    }
    #ifdef mqtt_discovery_prefix
        create_discovery_info(F("binary_sensor"), true, NULL, NULL, false, _high, _low);
    #endif
    return true;
}
