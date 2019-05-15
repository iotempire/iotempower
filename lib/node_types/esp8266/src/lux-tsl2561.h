// lux-tsl2561.h
// light sensor based on tsl2561 i2c chip

#ifndef _IOTEMPOWER_TSL2561_H_
#define _IOTEMPOWER_TSL2561_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c-device.h>

#include <SparkFunTSL2561.h>

class Lux_TSL2561 : public I2C_Device {
    private:
        SFE_TSL2561 *sensor = NULL;
        bool _gain;
        unsigned long last_read;
        int discardval = 5;
    public:
        Lux_TSL2561(const char* name, bool gain=false);
        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_BH1750_H_