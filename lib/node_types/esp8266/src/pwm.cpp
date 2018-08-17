// pwm.cpp
// change pwm parameters on a gpio port

#include "pwm.h"

Pwm::Pwm(const char* name, uint8_t pin, int frequency) 
        : Device(name) {
    _pin = pin;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int duty=payload.as_int();
            set_duty(duty);
            return true;
        }
    );
    add_subdevice(new Subdevice("frequency"));
    add_subdevice(new Subdevice("frequency/set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int frequency=payload.as_int();
            set_frequency(frequency);
            return true;
        }
    );
    pinMode(_pin, OUTPUT);
    //set(duty, frequency);
    set_duty(0);
}

void Pwm::set(int duty, int frequency) {
    _frequency = limit(frequency,2,5000);
    _duty = limit(duty,0,1023);
    measured_value(2).from(_frequency);
    measured_value(0).from(_duty);

    // frequency in square wave: number of intervals (low and high),
    // number of on and off switches
    // duty float (0..1): _duty/1023 as _duty is 0..1023
    uint32_t on_interval_length_us = (1000000L * _duty/1023L + frequency - 1)/ _frequency;
    if(!on_interval_length_us) { // never on
        stopWaveform(_pin);
        digitalWrite(_pin, 0);
    } else {
        uint32_t off_interval_length_us = (1000000L * (1023L-_duty)/1023L + frequency -1) / _frequency;
        if(!off_interval_length_us) { // never off
            stopWaveform(_pin);
            digitalWrite(_pin, 1);
        } else {
            startWaveform(_pin, on_interval_length_us, off_interval_length_us, 0);
        }
    }
}
