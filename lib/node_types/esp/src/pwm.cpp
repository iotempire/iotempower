// pwm.cpp
// change pwm parameters on a gpio port

#include "pwm.h"

Pwm::Pwm(const char* name, uint8_t pin, int frequency) 
        : Device(name, 1000) { // low pollrate 1ms
    _pwm = new _PWM(pin, frequency);
    if(!_pwm) {
        ulog(F("Can't register PWM. Not enough memory."));
        return;
    }
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice(F("set"),true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int duty=payload.as_int();
            set_duty(duty);
            return true;
        }
    );
    add_subdevice(new Subdevice(F("frequency")));
    add_subdevice(new Subdevice(F("frequency/set"),true))->with_receive_cb(
        [&] (const Ustring& payload) {
            int frequency=payload.as_int();
            set_frequency(frequency);
            return true;
        }
    );
    //set(duty, frequency);
    set_duty(0);
}

Pwm& Pwm::set(int duty, int frequency) {
    _frequency = limit(frequency,2,5000);
    _duty = limit(duty,0,1023);
    if(!started()) return *this;
    
    value(2).from(_frequency);
    value(0).from(_duty);
    _pwm->set(_duty, _frequency);
    return *this;
}
