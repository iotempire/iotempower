// gyro.h
// Header File for controlling gyroscopes like MPU6050 and MPU9250

#ifndef _ULNOIOT_GYRO_H_
#define _ULNOIOT_GYRO_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c_device.h>

#include <MPU6050.h> // here only this in cpp MPU6050_6Axis_MotionApps20.h

class Gyro_MPU6050 : public I2C_Device {
    private:
        bool _calibrate = false;
        unsigned long starttime;
        bool discard_done = false;
        MPU6050 *mpu6050 = NULL;
        uint16_t packetSize;
        uint8_t fifoBuffer[64]; // FIFO storage buffer
    public:
        Gyro_MPU6050(const char* name, bool calibrate_on_start=true);
        void start();
        bool measure();
};

#endif // _ULNOIOT_GYRO_H_