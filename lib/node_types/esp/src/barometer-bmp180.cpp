// bmp180.cpp
#include "barometer-bmp180.h"

Barometer_BMP180::Barometer_BMP180(const char* name) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice(F("temperature"))); // 0
    add_subdevice(new Subdevice(F("pressure"))); // 1
    add_subdevice(new Subdevice(F("altitude"))); // 2
    set_address(BMP085_ADDRESS);
}

void Barometer_BMP180::i2c_start() {
    sensor = new BMP085();

    if(sensor) {
        sensor->init(); // TODO: timeout
        _started = true;
    } else {
        ulog(F("Can't reserve memory for BMP085/BMP180."));
    }
}


bool Barometer_BMP180::measure() {

    float temperature = sensor->bmp085GetTemperature(sensor->bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    float pressure = sensor->bmp085GetPressure(sensor->bmp085ReadUP());//Get the temperature

    /*
    TODO: think if we need to put that in constructor
    To specify a more accurate altitude, enter the correct mean sea level
    pressure level.  For example, if the current pressure level is 1019.00 hPa
    enter 101900 since we include two decimal places in the integer value。
    */
    float altitude = sensor->calcAltitude(101900); 

    // adjust already due to precision TODO: revisit
    value(0).from(round(temperature*10)/10); // Celsius
    value(1).from((int)round(pressure/100)); // Hectopascal
    value(2).from((int)round(altitude/5)*5); // Meter
    return true;
}
