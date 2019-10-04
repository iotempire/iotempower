// _PWM.cpp

#include <_PWM.h>

_PWM:: _PWM(uint8_t pin, int frequency)
{
    _pin=pin;
//    pinMode(_pin, OUTPUT);
}

#define limit(nr, min, max) \
    ( (nr) < (min) ? (min):((nr) > (max) ? (max):(nr)) )

void _PWM::set(int duty, int frequency) {
    _frequency = limit(frequency,2,5000);
    _duty = limit(duty,0,1023);

    if(!esp32_pwm.attached())
        esp32_pwm.attachPin(_pin, 5000.0/frequency);
}

void  _PWM::set_duty(int duty)
{
    esp32_pwm.write(_duty);
}

void  _PWM::set_frequency(int frequency)
{
    esp32_pwm.adjustFrequency(5000.0/frequency);
}
