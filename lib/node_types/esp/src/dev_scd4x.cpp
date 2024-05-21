#include "dev_scd4x.h"

SCD4X_Device::SCD4X_Device(const char* name, TwoWire& wire, uint8_t i2cAddr)
    : I2C_Device(name), scd4x(&wire, i2cAddr) {
    set_pollrate(5000);                        // 5 seconds
    add_subdevice(new Subdevice(F("CO2ppm")));   // 0
    add_subdevice(new Subdevice(F("temp")));  // 1
    add_subdevice(new Subdevice(F("hmdt")));  // 1
    set_address(0x62);
}

void SCD4X_Device::i2c_start() {
    if (!scd4x.begin()) {
        ulog(F("SCD4X: Init failed!"));
        _in_init_phase = true;
    } else {
        ulog(F("SCD4X: Init success!"));
        _in_init_phase = false;
    }
    _started = true;
}

bool SCD4X_Device::measure() {
    if (_in_init_phase) {
        return false;  // Initialization phase, no measurement
    }

    if (scd4x.getDataReadyStatus()) {
        DFRobot_SCD4X::sSensorMeasurement_t data;
        scd4x.readMeasurement(&data);

        value(0).from(data.CO2ppm);    // CO2 in ppm
        value(1).from(data.temp);      // Temperature in deg C
        value(2).from(data.hmdt);  // Humidity as %RH

        ulog(F("SCD4X: New data read"));
        return true;
    } else {
        ulog(F("SCD4X: Data not ready"));
        return false;
    }
}
