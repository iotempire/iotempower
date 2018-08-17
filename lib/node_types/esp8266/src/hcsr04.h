// hcsr04.h
// Header File for trigger device (interrupt+counter on input)
//
// Refer to: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

#ifndef _HCSR04_H_
#define _HCSR04_H_

#include <Arduino.h>
#include <device.h>

// only sent out signal every 100ms
#define ULNOIOT_HCSR04_INTERVAL 100

class Hcsr04 : public Device {
    private:
        bool triggered = false;
        bool measured = false;
        unsigned long _timeout_us;
        unsigned long interval_start;
        unsigned long interval_end;
        unsigned long last_triggered;
        int _precision = 1;
        uint8_t _echo_pin;
        uint8_t _trigger_pin;
        void echo_changed();
    public:
        Hcsr04(const char* name, uint8_t trigger_pin, uint8_t echo_pin,  
            unsigned long timeout_us = 30000);
        Hcsr04& with_precision(int precision) {
            if(precision < 1) precision = 1;
            _precision = precision;
            return *this;
        }

        bool measure();
};


#endif // _HCSR04_H_