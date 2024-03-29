// dev_input_analog.cpp
#include "dev_input_analog.h"

static unsigned long last_read = micros()-5;

// bool Analog::measure() {
//     //    unsigned long current_time = micros();
//     // if(current_time - last_read < 3) return false;
//     // last_read = current_time; // why is that necessary?

//     Input_Base::measure(); // TODO: check

//     //value().from(read());
//     return true;
// }


void Analog::start() {
    Input_Base::start();
    if(_started) measure();
}
