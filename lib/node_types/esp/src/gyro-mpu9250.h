// gyro-mpu9250.h
// Header File for controlling the MPU9250 gyroscope

#ifndef _IOTEMPOWER_GYRO_MPU9250_H_
#define _IOTEMPOWER_GYRO_MPU9250_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

#include <quaternionFilters.h>
#include <MPU9250.h>

class Gyro_MPU9250 : public I2C_Device {
    private:
        MPU9250 *mpu9250 = NULL;
        // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
        // 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
        // - http://www.ngdc.noaa.gov/geomag-web/#declination
        double declination = 8.5; 

    public:
        Gyro_MPU9250(const char* name);
        void i2c_start();
        bool measure();
        Gyro_MPU9250& with_declination(double decl) {
            declination = decl;
            return *this;
        }
        Gyro_MPU9250& set_declination(double decl) {
            return with_declination(decl);
        }
};

#endif // _IOTEMPOWER_GYRO_MPU9250_H_