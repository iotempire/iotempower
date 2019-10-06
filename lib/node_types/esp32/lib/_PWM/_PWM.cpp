// _PWM.cpp

#include <_PWM.h>

_PWM:: _PWM(uint8_t pin, int frequency)
{
    _pin=pin;
    _frequency = frequency;
}

#define limit(nr, min, max) \
    ( (nr) < (min) ? (min):((nr) > (max) ? (max):(nr)) )

void _PWM::set(int duty, int frequency) {
    set_frequency(frequency); // first set new frequency to guarantee attached pin
    set_duty(duty);
}

void  _PWM::set_duty(int duty)
{
    _duty = limit(duty,0,1023);

    if(esp32_pwm.attached()) // only when attached
        esp32_pwm.write(_duty);
}

void  _PWM::set_frequency(int frequency)
{
    frequency = limit(frequency,2,5000);

    if(frequency != _frequency || !esp32_pwm.attached()) {
        if(esp32_pwm.attached()) // if necessary detach first
            esp32_pwm.detachPin(_pin);
        pinMode(_pin, OUTPUT); // this needs to happen here - a little late    
        esp32_pwm.attachPin(_pin, frequency, 10); // new attach to reset frequency
    }
}
