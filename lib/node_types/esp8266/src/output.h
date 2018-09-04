// output.h
// Header File for output device (simple gpio on/off, led, relay)

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <device.h>

class Output : public Device {
    private:
        const char* _high;
        const char* _low;
        int _pin;
    public:
        Output(const char* name, const int pin, 
                const char* high_command="on",
                const char* low_command="off" );
        void start();
        void high() { 
            if(started()) digitalWrite(_pin, 1);
            measured_value().from(_high);
        }
        void on() { high(); }
        void low() { 
            if(started()) digitalWrite(_pin, 0); 
            measured_value().from(_low);
        }
        void off() { low(); }

        bool is_high() {
            return value().equals(_high);
        }
        bool is_low() {
            return value().equals(_low);
        }
        // TODO: set output "floating"?
};


#endif // _OUTPUT_H_