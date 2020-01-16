// hx711.cpp
#include "hx711.h"

// TODO: this might be just an i2c device, check if it can be defined as one

Hx711::Hx711(const char* name, uint8_t sck_pin, uint8_t dout_pin,
                float calfactor, bool calibration)
        : Device(name, 100000) {
    _calfactor = calfactor;
    _sck_pin = sck_pin;
    _dout_pin = dout_pin;
    _calibration = calibration;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice(F("tare"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // any load here triggers tare
            ulog(F("HX711 tare requested."));
            tare();
            return true;
        });
    if(calibration) {
        add_subdevice(new Subdevice(F("calfactor"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // TODO: parse factor
            Ustring command(payload);
            if(command.starts_with(F("+"))) { // add to factor
                command.remove(1);
                _calfactor += command.as_float();
            } else if(command.starts_with(F("-"))) {
                command.remove(1);
                _calfactor -= command.as_float();                
            } else if(command.starts_with(F("="))) {
                command.remove(1);
                _calfactor = command.as_float();                
            } else {
                _calfactor = command.as_float();                
            }
            sensor.set_scale(_calfactor);
            return true;
        });
    }
}

#define stabilizing_time 2000 // for better tare precision

void Hx711::tare() {
    sensor.tare(5);
}

void Hx711::start() {
    ulog(F("Starting HX711 initialization, this will block 2s."));
    sensor.begin(_dout_pin, _sck_pin);
    if(!sensor.wait_ready_timeout(2000, 1)) {
        ulog("Problems accessing HX711, not starting driver.");
        return;
    }
    sensor.set_scale(_calfactor); // user set calibration factor (float)
    sensor.tare(10);
    _started = true;
    last_read = millis();
    ulog(F("HX711 initialized."));
}


bool Hx711::measure() {
    unsigned long current = millis();
    unsigned long delta = current - last_read;
    bool measured = false;

    //sensor->update();

    // if(first_tare_done && delta>=100) { // only deliver values after first tare done
    //     // only every 100ms
    if(sensor.is_ready()) {
    // go as often as pollrate allows
        float weight = sensor.get_units();
        if(abs(lastweight-weight)>_precision) {
            lastweight = weight;
            if(_calibration) {
                float factor = sensor.get_scale();
                value().printf("%0.1f %0.3f", weight, factor);
            } else {
                value().printf("%0.1f", weight);
            }
            measured = true;
            }
        last_read = current;
    }
    // if (sensor->getTareStatus() == true) {
    //    first_tare_done = true;
    //     ulog(F("HX711 tare complete"));
    // }
    return measured; // no new value available
}
