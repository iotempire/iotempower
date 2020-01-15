// emp_servo.cpp

#include "emp_servo.h"

void Emp_Servo::init(uint8_t pin, int min_us, int max_us, int duration) {
    _pin = pin;
    _min_us = min_us;
    _max_us = max_us;
    _duration = duration;
    do_register();
}

void Emp_Servo::start() {
    start_time = millis();
    _started = true;
}

void Emp_Servo::do_register() {
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice(F("set"),true))->with_receive_cb(
        [&] (const Ustring& payload) {
            process(payload);
            return true;
        }
    );
}

Emp_Servo& Emp_Servo::turn_to(int angle) {
    if(!started()) return *this;
    if(stopped) {
        _servo.attach(_pin, _min_us, _max_us);
        stopped = false;
    }
    start_time = millis();
    _servo.write(angle);
    _value = angle;
    value(0).from(angle);
    return *this;
}

Emp_Servo& Emp_Servo::set(int angle) {
    return turn_to(angle);
}

void Emp_Servo::process(const Ustring& angle) {
    int v = angle.as_int();
    turn_to(v);
}

bool Emp_Servo::measure() {
    if(_duration > 0 && (millis() - start_time > (unsigned long)_duration)) {
        stop();
    }
    return false;
}

void Emp_Servo::stop() {
    if(!started()) return;
    _servo.detach();
    stopped = true;
}
