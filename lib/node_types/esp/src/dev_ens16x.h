// dev_ens16x.h
// Header File for controlling the ENS16X CO2 sensors

#ifndef _IOTEMPOWER_ENS16X_H_
#define _IOTEMPOWER_ENS16X_H_

#include <i2c-device.h>


class Ens16x : public I2C_Device {
   private:
    bool _in_init_phase = false;

   public:
    Ens16x(const char* name);
    void i2c_start();
    bool measure();
};

#endif  // _IOTEMPOWER_ENS16X_H_
