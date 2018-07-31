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
                const char* high_command, const char* low_command ) :
            Device(name) {
            _high = high_command;
            _low = low_command;
            _pin = pin;
            pinMode(_pin,OUTPUT);
            add_subdevice(new Subdevice(""));
            add_subdevice(new Subdevice("set",true))->with_receive_cb(
                [&] (const Ustring& payload) {
                    if(payload.equals(_high) high();
                    else {
                        if(payload.equals(_low) low();
                        else return false;
                    }
                    return true;
                }
            );
            low(); // must be after subdevices as it uses global measured_value()
        }
        void high() { 
            digitalWrite(_pin, 1);
            measured_value().from(_high);
        }
        void on() { high(); }
        void low() { 
            digitalWrite(_pin, 0); 
            measured_value().from(_low);
        }
        void off() { low(); }
        // TODO: set output "floating"?
};


#endif // _OUTPUT_H_