// gesture-apds9960.cpp
#include "gesture-9960.h"

Gesture_APDS9960::Gesture_APDS9960(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice("")); // 0
    set_address(0x39);
}

void Gesture_APDS9960::start() {
    sensor = new APDS9960(get_address());

    if(sensor) {
        sensor->begin(); // TODO: timeout
        last_read = millis();
        _started = true;
    } else {
        ulog("Can't reserve memory for APDS9960.");
    }
}

// waltino: hier gehts dann los!!
bool Gesture_APDS9960::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 250) return false;
    last_read = current_time; 
//jetzt wie denn??
    float lux = sensor->readLightLevel();

    measured_value().from((int)round(lux)); // lx

    return true;
}
