// mpr121.cpp
#include "mpr121.h"

MPR121::MPR121(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice(F(""))); // 0
    // from adafruit example:
    // Default address is 0x5A, if tied to 3.3V its 0x5B
    // If tied to SDA its 0x5C and if SCL then 0x5D
    set_address(0x5A);
}

void MPR121::i2c_start() {
    sensor = new Adafruit_MPR121();

    if(sensor) {
        sensor->begin(get_address());
        _started = true;
    } else {
        ulog(F("Can't reserve memory for MPR121."));
    }
}


bool MPR121::measure() {

    uint16_t touched = sensor->touched();

    value(0).clear();
    for(int i=0; i<12; i++) {
        if(touched&2048) {
            value(0).add(F("1"));
        } else {
            value(0).add(F("0"));
        }
        touched = touched << 1;
    }
    
    return true;
}
