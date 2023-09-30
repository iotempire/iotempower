// mpr121.h
// Header File for controlling the MPR121 capacitive touch i2c sensor

#ifndef _IOTEMPOWER_MPR121_H_
#define _IOTEMPOWER_MPR121_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

#include <Adafruit_MPR121.h>

class MPR121 : public I2C_Device {
    private:
        Adafruit_MPR121 *sensor = NULL;
    public:
        MPR121(const char* name);
        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_MPR121_H_