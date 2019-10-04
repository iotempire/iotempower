/*
    _PWM.h
    abstraction library for using pwm on esp32
  */
  

#ifndef __PWM_h
#define __PWM_h

#include <Arduino.h>
#include <ESP32PWM.h>


class  _PWM
{
private:
    ESP32PWM esp32_pwm;
    uint8_t  _pin;
    int _frequency;
    int _duty;
public:
    _PWM(uint8_t pin, int frequency=1000);
    void set(int duty, int frequency);
    void set_duty(int duty);
    void set_frequency(int frequency);
};

#endif
