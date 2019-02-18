// gesture APDS9960.h
// gesture/color/proximity sensor based on apds9960 i2c chip

#ifndef _ULNOIOT_APDS9960_H_
#define _ULNOIOT_APDS9960_H_

#include <Arduino.h>
#include <Wire.h>
#include <i2c_device.h>

// also defined in fastled
#ifdef WAIT
#undef WAIT
#endif
#include <SparkFun_APDS9960.h>

class Gesture_Apds9960 : public I2C_Device {
    private:
        SparkFun_APDS9960 *sensor = NULL;
        unsigned long last_read;
        uint8_t _proximity_threshold; 
    public:
        Gesture_Apds9960(const char* name, uint8_t proximity_threshold=50);
        void start();
        bool measure();
};

#endif // _ULNOIOT_APDS9960_H_
