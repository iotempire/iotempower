// distance-vl53l0x.h
// I2C Time of flight distance sensor

#ifndef _IOTEMPOWER_VL53L0X_H_
#define _IOTEMPOWER_VL53L0X_H_

#include <Arduino.h>
#include <i2c-device.h>

#include <VL53L0X.h>
#include <filter.h> // TODO: check why this needs to be included explicitely

#define IOTEMPOWER_VL53L0X_MAXDIST 8190

class Distance_Vl53l0x : public I2C_Device {
    private:
        bool _calibrate = false;
        bool _long_range;
        bool _high_accuracy_slow;
        unsigned long starttime;
        bool discard_done = false;
        VL53L0X *sensor = NULL;
    public:
        Distance_Vl53l0x(const char* name, 
                    bool long_range=false, bool high_accuracy_slow=false);
        Distance_Vl53l0x& with_precision(int precision) {
            if(precision < 1) precision = 1;
            return (Distance_Vl53l0x&)with_filter_callback(*new Filter_Minchange<int>(precision));
        }
        Distance_Vl53l0x& precision(int precision) {
            return with_precision(precision);
        }

        void i2c_start();
        bool measure();
};

#endif // _IOTEMPOWER_VL53L0X_H_