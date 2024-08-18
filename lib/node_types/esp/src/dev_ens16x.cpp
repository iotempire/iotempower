// dev_ens16x.cpp
//WIP
#include "dev_ens16x.h"

Ens16x::Ens16x(const char *name) : I2C_Device(name) {
    set_pollrate(1000); // 1 seconds
    add_subdevice(new Subdevice(F("tvoc")));  // 0
    add_subdevice(new Subdevice(F("co2eq"))); // 1
    set_address(0x58);
}

void Ens16x::i2c_start() {
    _in_init_phase = true;
    _started = true;
}

bool Ens16x::measure() {

    if (_in_init_phase) {
       
        if (err == STATUS_OK) {
            /* Read H2 and Ethanol signal in the way of blocking */
            err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                                    &scaled_h2_signal);
            if (err == STATUS_OK) {
                ulog(F("ENS16x: get ram signal!"));
            
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
            ulog(F("ENS16x: error reading IAQ values"));
            return false;
        }
    }
    return true;
}