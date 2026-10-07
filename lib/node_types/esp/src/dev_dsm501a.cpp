// dsm501a.cpp

// Important to include Functional Interrupt as else it does not allow to
// define a class-based (object specific) interrupt.
#include <FunctionalInterrupt.h>
#include "dev_dsm501a.h"

DSM501A::DSM501A(const char* name, uint8_t pm1_pin, uint8_t pm25_pin) :
    Device(name, 500000) {
    _pm1_pin = pm1_pin;
    _pm25_pin = pm25_pin;
    add_subdevice(new Subdevice(F("pm1")));
    add_subdevice(new Subdevice(F("pm25")));
}

void DSM501A::pm1_changed() {
    uint32_t now_us = micros();
    if(digitalRead(_pm1_pin) == LOW) {
        if(!_pm1_low_active) {
            _pm1_low_active = true;
            _pm1_low_started_us = now_us;
        }
    } else if(_pm1_low_active) {
        _pm1_lowpulse_us += now_us - _pm1_low_started_us;
        _pm1_low_active = false;
    }
}

void DSM501A::pm25_changed() {
    uint32_t now_us = micros();
    if(digitalRead(_pm25_pin) == LOW) {
        if(!_pm25_low_active) {
            _pm25_low_active = true;
            _pm25_low_started_us = now_us;
        }
    } else if(_pm25_low_active) {
        _pm25_lowpulse_us += now_us - _pm25_low_started_us;
        _pm25_low_active = false;
    }
}

float DSM501A::calculate_concentration(uint32_t lowpulse_us) const {
    float ratio = (float)lowpulse_us / ((float)_sampletime_ms * 10.0f);
    float concentration = 0.001915f * ratio * ratio + 0.09522f * ratio - 0.04884f;
    if(concentration < 0.0f) concentration = 0.0f;
    return concentration;
}

void DSM501A::start() {
    pinMode(_pm1_pin, INPUT);
    pinMode(_pm25_pin, INPUT);
    _sample_started_ms = millis();

    attachInterrupt(digitalPinToInterrupt(_pm1_pin), [&] () { pm1_changed(); }, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pm25_pin), [&] () { pm25_changed(); }, CHANGE);

    _started = true;
}

bool DSM501A::measure() {
    uint32_t now_ms = millis();
    if((uint32_t)(now_ms - _sample_started_ms) < _sampletime_ms) {
        return false;
    }

    // Snapshot and reset both low-pulse accumulators at the sample boundary.
    // If a pulse is still active, split it at "now" so the next window keeps
    // measuring seamlessly without losing time.
    uint32_t pm1_lowpulse_us = 0;
    uint32_t pm25_lowpulse_us = 0;
    uint32_t now_us = micros();

    noInterrupts();
    pm1_lowpulse_us = _pm1_lowpulse_us;
    pm25_lowpulse_us = _pm25_lowpulse_us;

    if(_pm1_low_active) {
        pm1_lowpulse_us += now_us - _pm1_low_started_us;
        _pm1_low_started_us = now_us;
    }
    if(_pm25_low_active) {
        pm25_lowpulse_us += now_us - _pm25_low_started_us;
        _pm25_low_started_us = now_us;
    }

    _pm1_lowpulse_us = 0;
    _pm25_lowpulse_us = 0;
    interrupts();

    _sample_started_ms = now_ms;

    value(0).printf("%.3f", calculate_concentration(pm1_lowpulse_us));
    value(1).printf("%.3f", calculate_concentration(pm25_lowpulse_us));
    return true;
}
