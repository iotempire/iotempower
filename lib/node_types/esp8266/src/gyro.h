// gyro.h
// Header File for controlling gyroscopes like MPU6050 and MPU9250

#ifndef _ULNOIOT_GYRO_H_
#define _ULNOIOT_GYRO_H_

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

#include <i2c_device.h>

class Gyro_MPU6050 : public I2C_Device {
    private:
        bool _calibrate = false;
        MPU6050 *mpu6050 = NULL;
        unsigned long _read_interval = 1000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
    public:
        Gyro_MPU6050(const char* name, bool calibrate_on_start=true);
        void start();
        bool measure();
};

#endif // _ULNOIOT_GYRO_H_