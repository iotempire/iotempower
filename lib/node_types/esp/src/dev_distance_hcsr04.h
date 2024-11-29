// hcsr04.h
// ultrasonic distance sensor
//
// Refer to: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

#ifndef _HCSR04_H_
#define _HCSR04_H_

#include <Arduino.h>
#include <device.h>
#include <filter.h> // TODO: check why this needs to be included explicitely

// only sent out signal every 50ms (when using 40ms there is too much stray)
#define IOTEMPOWER_HCSR04_INTERVAL 50
// should be odd for true median finding
#define IOTEMPOWER_HCSR04_BUFFER_SIZE 9
#define IOTEMPOWER_HCSR04_MAX_DISTANCE 4000 // 4m maximum distance

class Hcsr04 : public Device {
    private:
        const unsigned long timeout_us = 30000; // 30 ms timeout default
        unsigned int distance_buffer[IOTEMPOWER_HCSR04_BUFFER_SIZE];
        int distance_buffer_fill = 0;
        unsigned long distance_sum = 0;
        bool triggered = false;
        bool trigger_started = false;
        bool measured = false;
        unsigned long _timeout_us;
        unsigned long interval_start;
        unsigned long interval_end;
        unsigned long last_triggered;
        uint8_t _echo_pin;
        uint8_t _trigger_pin;
        void echo_changed();
    public:
        Hcsr04(const char* name, uint8_t trigger_pin, uint8_t echo_pin);
        Hcsr04& with_precision(int precision) {
            if(precision < 1) precision = 1;
            return (Hcsr04&)with_filter_callback(*new Filter_Minchange<int>(precision));
        }
        Hcsr04& precision(int precision) {
            return with_precision(precision);
        }
        void start();
        bool measure();
};


#endif // _HCSR04_H_