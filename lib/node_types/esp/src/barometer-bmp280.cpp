// bmp280.cpp
#include "barometer-bmp280.h"

Barometer_BMP280::Barometer_BMP280(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice(F("temperature"))); // 0
    add_subdevice(new Subdevice(F("pressure"))); // 1
    add_subdevice(new Subdevice(F("altitude"))); // 2
    set_address(BMP280_ADDRESS);
}

void Barometer_BMP280::i2c_start() {
    sensor = new BMP280();

    if(sensor) {
        if(!sensor->init()) { // TODO: timeout
            Serial.println(F("BMP280 not connected or broken!"));
        } else {
            _started = true;
        }
    } else {
        ulog(F("Can't reserve memory for bmp280."));
    }
}


bool Barometer_BMP280::measure() {
    float pressure;

    // adjust already due to precision TODO: revisit
    value(0).from(round(sensor->getTemperature()*10)/10); // Celsius
    pressure = sensor->getPressure();
    value(1).from(((int)round(pressure/100))); // Hecto-Pascal
    value(2).from((int)round(sensor->calcAltitude(pressure))); // meter
    return true;
}
