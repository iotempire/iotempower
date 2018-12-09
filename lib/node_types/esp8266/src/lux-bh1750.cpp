// lux-bh1750.cpp
#include "lux-bh1750.h"

Lux_BH1750::Lux_BH1750(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice("")); // 0
    set_address(0x23);
}

void Lux_BH1750::start() {
    sensor = new BH1750(get_address());

    if(sensor) {
        sensor->begin(); // TODO: timeout
        _started = true;
    } else {
        ulog("Can't reserve memory for BH1750.");
    }
}


bool Lux_BH1750::measure() {

    float lux = sensor->readLightLevel();

    measured_value().from((int)round(lux)); // lx

    return true;
}
