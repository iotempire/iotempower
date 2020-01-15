// lux-bh1750.cpp
#include "lux-bh1750.h"

Lux_BH1750::Lux_BH1750(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice("")); // 0
    set_address(0x23);
}

void Lux_BH1750::i2c_start() {
    sensor = new BH1750(get_address());

    if(sensor) {
        sensor->begin(); // TODO: timeout
        last_read = millis();
        _started = true;
    } else {
        ulog(F("Can't reserve memory for BH1750."));
    }
}


bool Lux_BH1750::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 1000) return false;
    last_read = current_time; 

    float lux = sensor->readLightLevel();

    value().from((int)round(lux)); // lx

    return true;
}
