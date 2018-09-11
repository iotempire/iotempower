// i2c_device.cpp
// author: ulno
// created: 2018-09-11

#include "i2c_device.h"

void I2C_Device::measure_init() {
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(clock_speed);
}
