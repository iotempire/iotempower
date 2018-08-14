// input.h
// Header File for input device (simple gpio, button, contact)

#ifndef _INPUT_H_
#define _INPUT_H_

#include <Arduino.h>
#include <device.h>

class Input : public Device {
    private:
        const char* _high;
        const char* _low;
        int _pin;
        int _threshold;
        bool _pull_up = true;
        int debouncer = 0;
        void reinit() {
            if(_pull_up) {
                pinMode(_pin, INPUT_PULLUP);
            } else {
                pinMode(_pin, INPUT);
            }
            measure();
        }
    public:
        Input(const char* name, int pin, const char* high, const char* low ) :
            Device(name) {
            _high = high;
            _low = low;
            _pin = pin;
            with_threshold(0);
            add_subdevice(new Subdevice(""));
            reinit();
        }
        bool measure();
        Input& with_pull_up(bool pull_up=true) {
            _pull_up = pull_up;
            reinit();
            return *this;
        }
        Input& with_threshold(int threshold) {
            _threshold = threshold + 1;
            debouncer = digitalRead(_pin) * _threshold;
            return *this;
        }
};


#endif // _INPUT_H_