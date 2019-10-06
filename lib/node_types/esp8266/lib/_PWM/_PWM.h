/*
    _PWM.h
    abstraction library for using pwm on esp8266 and esp32
  */
  

#ifndef __PWM_h
#define __PWM_h

#include <Arduino.h>


class  _PWM
{
private:
    uint8_t  _pin;
    int _frequency;
    int _duty;
    bool started = false;
public:
    _PWM(uint8_t pin, int frequency=1000);
    void set(int duty, int frequency);
    void set_duty(int duty);
    void set_frequency(int frequency);
};

#endif
