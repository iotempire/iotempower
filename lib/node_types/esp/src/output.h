// output.h
// Header File for output device (simple gpio on/off, led, relay)

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <device.h>

class Output : public Device {
    private:
        const char* _high;
        const char* _low;
        bool _inverted;
        bool _light = false;
        int _pin;
    public:
        Output(const char* name, const int pin, 
                const char* high_command=str_on,
                const char* low_command=str_off,
                bool inverted=false );
        void start();
        Output& invert() {
            _inverted = true;
            return *this;
        }
        Output& inverted() {
            return invert();
        }
        Output& light() { // for discovery
            _light = true;
            return *this;
        }
        Output& high() { 
            if(started()) digitalWrite(_pin, _inverted?0:1);
            value().from(_high);
            return *this;
        }
        Output& on() { return high(); }
        Output& low() { 
            if(started()) digitalWrite(_pin, _inverted?1:0); 
            value().from(_low);
            return *this;
        }
        Output& off() { return low(); }

        bool is_high() {
            return get_last_confirmed_value().equals(_high);
        }
        bool is_low() {
            return get_last_confirmed_value().equals(_low);
        }
        Output& set(const char* value) {
            Ustring v(value);
            if(v.equals(_high)) high();
            else {
                if(v.equals(_low)) low();
            }
            return *this;
        }
        void toggle() {
            if(value().equals(_high)) set(_low);
            else set(_high);
        }

        // TODO: set output "floating"?
};


#endif // _OUTPUT_H_