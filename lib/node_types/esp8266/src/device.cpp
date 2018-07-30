// device.cpp
// author: ulno
// created: 2018-07-16

#include <Arduino.h>
#include "device.h"

bool Subdevice::call_receive_cb(Ustring& payload) {
    Serial.print("Calling receive callback. ");
    if(receive_cb != NULL) {
        Serial.println(payload.as_cstr());
        return receive_cb(payload);
    }
    Serial.println("!failure!");
    return false;
}

bool Device::update() {
    if(!measure()) {  // measure new value or trigger physical update
        // re-use last value(s), if measurement not successful
        subdevices.for_each( [](Subdevice& sd) {
            sd.measured_value.copy(sd.current_value);
            return true; // continue loop
        } );
    }
    // a current value is now in measured_value
    // check if it needs to be filtered
    if(filter_cb != NULL && ! filter_cb(*this)) {
        // if filter defined but indicates to ignore the current measurement
        return false; // end here with no update
    }
    // The measured value is an actual, valid new measurement
    // check if the value has changed/is updated

    bool updated = false;
    subdevices.for_each( [&] (Subdevice& sd) {
        if(!sd.measured_value.equals(sd.current_value)) {
            sd.current_value.copy(sd.measured_value);
            updated = true;
        }
        return true; // continue loop
    } );
    if(updated) {
        if(on_change_cb != NULL) {
            on_change_cb(*this);
        }
        return true; // signal "real" (with change) update happened
    }
    return false; // not changed
}

// TODO: why can't I make this const/const?
Ustring& Device::value(unsigned long index) {
    Subdevice* sd = subdevice(index);
    if(sd == NULL) {
        Serial.print("Error in value(). Working on Device: ");
        Serial.println(name.as_cstr());
        controlled_crash("no subdevice");
    }
    return sd->value();
}

Ustring& Device::measured_value(unsigned long index) {
    Subdevice* sd = subdevice(index);
    if(sd == NULL) {
        Serial.print("Error in measured_value(). Working on Device: ");
        Serial.println(name.as_cstr());
        controlled_crash("no subdevice");
    }
    return sd->measured_value;
}
