// _PWM.cpp

#include <_PWM.h>
#include "core_esp8266_waveform.h"


_PWM:: _PWM(uint8_t pin, int frequency)
{
    _pin=pin;
    _frequency = frequency;
}

#define limit(nr, min, max) \
    ( (nr) < (min) ? (min):((nr) > (max) ? (max):(nr)) )

void _PWM::set(int duty, int frequency) {
    if(!started) { // late start - at first access
        pinMode(_pin, OUTPUT);
        started = true;
    }

    _frequency = limit(frequency,2,5000);
    _duty = limit(duty,0,1023);

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

void  _PWM::set_duty(int duty)
{
    set(duty,_frequency);
}

void  _PWM::set_frequency(int frequency)
{
    set(_duty,frequency);
}
