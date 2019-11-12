// gyro.h
// Header File for controlling the MPU6050 gyroscope

#ifndef _IOTEMPOWER_GYRO_H_
#define _IOTEMPOWER_GYRO_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

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
        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_GYRO_H_