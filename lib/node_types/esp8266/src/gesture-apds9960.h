// gesture APDS9960.h
// gesture/colour/proximity sensor based on apds9960 i2c chip

#ifndef _ULNOIOT_APDS9960_H_
#define _ULNOIOT_APDS9960_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c_device.h>

#include <APDS9960.h>

class Gesture_APDS9960 : public I2C_Device {
    private:
        APDS9960 *sensor = NULL;
        unsigned long last_read;
    public:
        GESTURE_APDS9960(const char* name);
        void start();
        bool measure();
};

#endif // _ULNOIOT_APDS9960_H_
