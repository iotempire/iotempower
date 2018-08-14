// trigger.h
// Header File for trigger device (interrupt+counter on input)

#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include <Arduino.h>
#include <device.h>

class Trigger : public Device {
    private:
        unsigned long last_counter;
        bool _falling;
        bool _rising;
        int _pin;
        bool _pull_up = true;
        void reinit();
    public:
        Trigger(const char* name, int pin, bool rising=true, bool falling=true) :
            Device(name) {
            _pin = pin;
            _rising = rising;
            _falling = falling;
            add_subdevice(new Subdevice(""));
            reinit();
        }
        bool measure();
        Trigger& with_pull_up(bool pull_up=true) {
            _pull_up = pull_up;
            reinit();
            return *this;
        }
        Trigger& with_falling(bool falling=true) {
            _falling = falling;
            reinit();
            return *this;
        }
        Trigger& with_rising(bool rising=true) {
            _rising = rising;
            reinit();
            return *this;
        }
};


#endif // _TRIGGER_H_