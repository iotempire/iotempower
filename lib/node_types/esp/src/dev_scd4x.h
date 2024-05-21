#ifndef _IOTEMPOWER_SCD4X_H_
#define _IOTEMPOWER_SCD4X_H_

#include <i2c-device.h>
#include "DFRobot_SCD4X.h"  // Include the SCD4X library

class SCD4X_Device : public I2C_Device {
private:
    DFRobot_SCD4X scd4x;  // SCD4X sensor object
    bool _in_init_phase = true;

public:
    SCD4X_Device(const char* name, TwoWire &wire = Wire, uint8_t i2cAddr = SCD4X_I2C_ADDR);
    void i2c_start();
    bool measure();
};

#endif // _IOTEMPOWER_SCD4X_H_
