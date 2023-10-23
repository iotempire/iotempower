// dev_output.cpp

#include "dev_output.h"

Output::Output(const char* name, const int pin, 
        const char* high_command, const char* low_command,
        bool inverted ) :
    Device(name, 1000) { // very low pollrate (1ms) to react fast
    _high = high_command;
    _low = low_command;
    _pin = pin;
    _inverted = inverted;
    add_subdevice(new Subdevice());
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set(payload.as_cstr());
            return true;
        }
    );
// don't switch until started    low(); // must be after subdevices as it uses global value()
}

void Output::start() {
    #ifdef mqtt_discovery_prefix
        // late creation in start as _light might not be defined in constructor yet
        create_discovery_info(_light?F("light"):F("switch"), 
            true, _light?NULL:_high, _light?NULL:_low,
            true, _high, _low);
    #endif
    
    pinMode(_pin,OUTPUT);
    _started = true;
    if(value().equals(_high)) high();
    else low();
}
