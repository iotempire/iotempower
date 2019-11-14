// mpr121.h
// Header File for controlling the mpr121 capacitive touch sensor

#ifndef _IOTEMPOWER_MPR121_H_
#define _IOTEMPOWER__H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

#include <BMP085.h>

class Barometer_BMP180 : public I2C_Device {
    private:
        BMP085 *sensor = NULL;
    public:
        Barometer_BMP180(const char* name);
        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_BMP180_H_