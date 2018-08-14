// trigger.h
// Header File for trigger device (interrupt+counter on input)

#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include <Arduino.h>
#include <device.h>

class Edge_Counter : public Device {
    private:
        bool triggered = true;
        unsigned long counter = 0;
        bool _falling;
        bool _rising;
        int _pin;
        bool _pull_up = true;
        void reinit();
        void count();
    public:
        Edge_Counter(const char* name, int pin, bool rising=true, bool falling=true);
        bool measure();
        Edge_Counter& with_pull_up(bool pull_up=true) {
            _pull_up = pull_up;
            reinit();
            return *this;
        }
};


#endif // _TRIGGER_H_