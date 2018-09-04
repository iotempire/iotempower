// output.cpp

#include "output.h"

Output::Output(const char* name, const int pin, 
        const char* high_command, const char* low_command ) :
    Device(name) {
    _high = high_command;
    _low = low_command;
    _pin = pin;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice("set",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            if(payload.equals(_high)) high();
            else {
                if(payload.equals(_low)) low();
                else return false;
            }
            return true;
        }
    );
    low(); // must be after subdevices as it uses global measured_value()
}

void Output::start() {
    pinMode(_pin,OUTPUT);
    _started = true;
    if(measured_value().equals(_high)) high();
    else low();
}
