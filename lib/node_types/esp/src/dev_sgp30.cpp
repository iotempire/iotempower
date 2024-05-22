// dev_sgp30.cpp
#include "dev_sgp30.h"

Sgp30::Sgp30(const char *name) : I2C_Device(name) {
    set_pollrate(1000); // 1 seconds
    add_subdevice(new Subdevice(F("tvoc")));  // 0
    add_subdevice(new Subdevice(F("co2eq"))); // 1
    set_address(0x58);
}

void Sgp30::i2c_start() {
    _in_init_phase = true;
    _started = true;
}

bool Sgp30::measure() {
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
    } else { // normal measure
        err = 0;
        u16 tvoc_ppb, co2_eq_ppm;
        err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        if (err == STATUS_OK) {
            value(0).from(tvoc_ppb);   // ppbm
            value(1).from(co2_eq_ppm); // ppm
        } else {
            ulog(F("SGP30: error reading IAQ values"));
            return false;
        }
    }
    return true;
}