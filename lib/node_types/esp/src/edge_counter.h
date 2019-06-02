// edge_counter.h
// Header File for edge counter device (interrupt+counter on input edge)

#ifndef _EDGE_COUNTER_H_
#define _EDGE_COUNTER_H_

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
        void start();
        bool measure();
        Edge_Counter& with_pull_up(bool pull_up=true) {
            _pull_up = pull_up;
            reinit();
            return *this;
        }
};


#endif // _EDGE_COUNTER_H_