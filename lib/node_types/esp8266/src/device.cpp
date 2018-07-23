// device.cpp
// author: ulno
// created: 2018-07-16

#include "device.h"

bool Device::update() {
    if(!measure()) {  // measure new value or trigger physical update
        // re-use last value, if measurement not successful
        measured_value.copy(current_value);
    }
    // a current value is now in measured_value
    // check if it needs to be filtered
    if(filter_cb != NULL && ! filter_cb(measured_value)) {
        // if filter defined but indicates to ignore the current measurement
        return false; // end here with no update
    }
    // The measured value is an actual, valid new measurement
    // check if the value has changed/is updated
    if(!measured_value.equals(current_value)) {
        current_value.copy(measured_value);
        if(on_change_cb != NULL) {
            on_change_cb(*this, current_value);
        }
        return true; // signal "real" (with change) update happened
    }
    return false; // not changed
}