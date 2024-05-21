// dev_scd4x.h
// Header File for controlling the SCD4X CO2 sensors

#ifndef _IOTEMPOWER_SCD4X_H_
#define _IOTEMPOWER_SCD4X_H_

#include <i2c-device.h>  // Include the I2C device library
#include "DFRobot_SCD4X.h"  // Include the SCD4X library

class Scd4x : public I2C_Device {
   private:
    bool _in_init_phase = false;

   public:
    Scd4x(const char* name);
    void i2c_start();
    bool measure();
};

#endif  // _IOTEMPOWER_SCD4X_H_
