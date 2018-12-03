// barometer-bmp280.h
// Header File for controlling the BMP280 barometer

#ifndef _ULNOIOT_GYRO_H_
#define _ULNOIOT_GYRO_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c_device.h>

#include <Seeed_BMP280.h>

class Barometer_BMP280 : public I2C_Device {
    private:
        BMP280 *sensor = NULL;
    public:
        Barometer_BMP280(const char* name);
        void start();
        bool measure();
};

#endif // _ULNOIOT_GYRO_H_