// barometer-bmp180.h
// Header File for controlling the BMP180 barometer

#ifndef _ULNOIOT_BMP180_H_
#define _ULNOIOT_BMP180_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c_device.h>

#include <BMP085.h>

class Barometer_BMP180 : public I2C_Device {
    private:
        BMP085 *sensor = NULL;
    public:
        Barometer_BMP180(const char* name);
        void start();
        bool measure();
};

#endif // _ULNOIOT_BMP180_H_