// dev_sgp30.h
// Header File for controlling the SGP30 IAQ sensor

#ifndef _IOTEMPOWER_SGP30_H_
#define _IOTEMPOWER_SGP30_H_

#include <i2c-device.h>
#include <sgp30.h>
#include "sensirion_common.h"

class Sgp30 : public I2C_Device {
   private:
    bool _in_init_phase = false;

   public:
    Sgp30(const char* name);
    void i2c_start();
    bool measure();
};

#endif  // _IOTEMPOWER_SGP30_H_