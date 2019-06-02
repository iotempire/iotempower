// servo.cpp

#include "servo.h"

void Servo::init(uint8_t pin, int min_us, int max_us, int duration) {
    _pin = pin;
    _min_us = min_us;
    _max_us = max_us;
    _duration = duration;
    do_register();
}

void Servo::start() {
    start_time = millis();
    _started = true;
}

void Servo::do_register() {
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            process(payload);
            return true;
        }
    );
}

Servo& Servo::turn_to(int value) {
    if(!started()) return *this;
    if(stopped) {
        _servo.attach(_pin, _min_us, _max_us);
        stopped = false;
    }
    start_time = millis();
    _servo.write(value);
    _value = value;
    measured_value(0).from(value);
    return *this;
}

Servo& Servo::set(int value) {
    return turn_to(value);
}

void Servo::process(const Ustring& value) {
    int v = value.as_int();
    turn_to(v);
}

bool Servo::measure() {
    if(_duration > 0 && (millis() - start_time > (unsigned long)_duration)) {
        stop();
    }
    return false;
}

void Servo::stop() {
    if(!started()) return;
    _servo.detach();
    stopped = true;
}
