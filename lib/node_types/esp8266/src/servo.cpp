// servo.cpp

#include "servo.h"

void Servo::init(uint8_t pin, int min_us, int max_us, int duration) {
    _pin = pin;
    _min_us = min_us;
    _max_us = max_us;
    _duration = duration;
    start_time = millis();
}

void Servo::do_register() {
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int value=payload.as_int();
            set(value);
            return true;
        }
    );
}

void Servo::turn_to(int value) {
    if(stopped) {
        _servo.attach(_pin, _min_us, _max_us);
        stopped = false;
    }
    start_time = millis();
    _servo.write(value);
    _value = value;
    measured_value(0).from(value);
}

void Servo::set(int value) {
    turn_to(value);
}

bool Servo::measure() {
    if(_duration > 0 && (millis() - start_time > (unsigned long)_duration)) {
        stop();
    }
    return false;
}

void Servo::stop() {
    _servo.detach();
    stopped = true;
}
