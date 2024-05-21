#include "dev_scd4x.h"

Scd4x::SCD4X_Device(const char* name) : I2C_Device(name) {
    set_pollrate(5000);                         // 5 seconds
    add_subdevice(new Subdevice(F("CO2ppm")));  // 0
    add_subdevice(new Subdevice(F("temp")));    // 1
    add_subdevice(new Subdevice(F("hmdt")));    // 1
    set_address(0x62);
}

void Scd4x::i2c_start() {
    _in_init_phase = true;
    _started = true;
}

bool Scd4x::measure() {
    if (_in_init_phase) {
        return false;  // Initialization phase, no measurement
    }

    if (Scd4x.getDataReadyStatus()) {
        DFRobot_SCD4X::sSensorMeasurement_t data;
        Scd4x.readMeasurement(&data);

        value(0).from(data.CO2ppm);  // CO2 in ppm
        value(1).from(data.temp);    // Temperature in deg C
        value(2).from(data.hmdt);    // Humidity as %RH

        ulog(F("SCD4X: New data read"));
        return true;
    } else {
        ulog(F("SCD4X: Data not ready"));
        return false;
    }
    return true;

    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;

    if (_in_init_phase) {
        /*
            Init module, Reset all baseline, The initialization takes up to
           around 15 seconds, during which all APIs measuring IAQ(Indoor air
           quality ) output will not change. Default value is 400(ppm) for co2,
           0(ppb) for tvoc
        */
        err = sgp_probe();
        if (err == STATUS_OK) {
            /* Read H2 and Ethanol signal in the way of blocking */
            err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                                    &scaled_h2_signal);
            if (err == STATUS_OK) {
                ulog(F("SGP30: get ram signal!"));
            } else {
                ulog(F("SGP30: error reading signals"));
            }
            err = sgp_iaq_init();
            if (err == STATUS_OK) {
                ulog(F("SGP30: init success!"));
                _in_init_phase = false;
            } else {
                ulog(F("SGP30: init failed!"));
            }
        }
        return false;
    } else {  // normal measure
        err = 0;
        u16 tvoc_ppb, co2_eq_ppm;
        err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        if (err == STATUS_OK) {
            value(0).from(tvoc_ppb);    // ppbm
            value(1).from(co2_eq_ppm);  // ppm
        } else {
            ulog(F("SGP30: error reading IAQ values"));
            return false;
        }
    }
}
