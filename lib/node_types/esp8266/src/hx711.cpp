// hx711.cpp
#include "hx711.h"


Hx711::Hx711(const char* name, uint8_t sck_pin, uint8_t dt_pin,
                float calfactor, bool calibration)
        : Device(name) {
    _calfactor = calfactor;
    _sck_pin = sck_pin;
    _dt_pin = dt_pin;
    _calibration = calibration;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("tare"))->with_receive_cb(
        [&] (const Ustring& payload) {
            // any load here triggers tare
            ulog("HX711 tare requested.");
            sensor->tareNoDelay();
            return true;
        });
    if(calibration) {
        add_subdevice(new Subdevice("calfactor"))->with_receive_cb(
        [&] (const Ustring& payload) {
            // TODO: parse factor
            Ustring command(payload);
            if(command.starts_with("+")) { // add to factor
                command.remove(1);
                _calfactor += command.as_float();
            } else if(command.starts_with("+")) {
                command.remove(1);
                _calfactor -= command.as_float();                
            } else {
                _calfactor = command.as_float();                
            }
            return true;
        });
    }
}

#define stabilizing_time 2000 // for better tare precision

void Hx711::start() {
    ulog("Starting HX711 initialization, this will block 3s.");
    sensor = new HX711_ADC(_dt_pin, _sck_pin);
    if(sensor) {
        sensor->start(stabilizing_time);
        sensor->setCalFactor(_calfactor); // user set calibration factor (float)
        _started = true;
        last_read = millis();
        ulog("HX711 initialized.");
    } else {
        ulog("Trouble reserving memory for HX711.");
    }
}


bool Hx711::measure() {
    unsigned long current = millis();
    unsigned long delta = current - last_read;
    bool measured = false;

    sensor->update();

    if(delta>=250) {
        float weight = sensor->getData();
        if(_calibration) {
            float factor = sensor->getCalFactor();
            measured_value().printf("%0.1f %0.3f", weight, factor);
        } else {
            measured_value().printf("%0.1f", weight);
        }
        measured = true;
        last_read = current;
    }
    if (sensor->getTareStatus() == true) {
        ulog("HX711 tare complete");
    }
    return measured; // no new value available
}
