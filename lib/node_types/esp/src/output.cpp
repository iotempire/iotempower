// output.cpp

#include "output.h"

Output::Output(const char* name, const int pin, 
        const char* high_command, const char* low_command,
        bool inverted ) :
    Device(name) {
    _high = high_command;
    _low = low_command;
    _pin = pin;
    _inverted = inverted;
    add_subdevice(new Subdevice(""));
    add_subdevice(new Subdevice(F("set"),true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set(payload.as_cstr());
            return true;
        }
    );
    #ifdef mqtt_discovery_prefix
        create_discovery_info(F("switch"), 
            true, high_command, low_command,
            true, high_command, low_command);
    #endif
    low(); // must be after subdevices as it uses global measured_value()
}

void Output::start() {
    pinMode(_pin,OUTPUT);
    _started = true;
    if(measured_value().equals(_high)) high();
    else low();
}
