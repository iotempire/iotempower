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
        bool _inverted;
        int _pin;
        int _threshold;
        bool _pull_up = true;
        int debouncer = 0;
        void reinit() {
            if(!started()) return;
            if(_pull_up) {
                pinMode(_pin, INPUT_PULLUP);
            } else {
                pinMode(_pin, INPUT);
            }
            measure();
        }
    public:
        Input(const char* name, int pin, 
            const char* high=str_on, const char* low=str_off, bool inverted = false) :
            Device(name, 1000) { // faster default pollrate (1ms) than other devices
            _high = high;
            _low = low;
            _pin = pin;
            _inverted = inverted;
            with_threshold(0);
            add_subdevice(new Subdevice());
        }
        void start() {
            _started = true;
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
        Input& debounce(int threshold) {
            return with_threshold(threshold);
        }
        Input& with_debounce(int threshold) {
            return with_threshold(threshold);
        }
        Input& invert() {
            _inverted = true;
            return *this;
        }
        Input& inverted() {
            _inverted = true;
            return *this;
        }
        bool is_high() {
            return value().equals(_inverted?_low:_high);
        }
        bool is_on() { return is_high(); }
        bool is_low() {
            return value().equals(_inverted?_high:_low);
        }
        bool is_off() { return is_low(); }
        bool is(const char* test_value) {
            return value().equals(test_value);
        }
};

#endif // _INPUT_H_