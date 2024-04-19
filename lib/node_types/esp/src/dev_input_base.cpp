// dev_input_base.cpp
#include "dev_input_base.h"

void set_precision_interval(long interval_us, long unprecision_interval_us=-1);

Input_Base& Input_Base::precise_buffer(unsigned long interval_ms, unsigned int reads) {
    pollrate_us(interval_ms*1000L/reads);
    set_precision_interval(interval_ms*1000L);
    _precise_reads = reads;
    demand_precision(); // make sure you end in precision interval
    return *this;
}

void Input_Base::start() {
    if(_num_mux_pins > 0) {
        for (int i = 0; i < _num_mux_pins; i++) {
            pinMode(_mux_pins[i], OUTPUT);
            digitalWrite(_mux_pins[i], LOW);
        }
    }

    if(_precise_reads > 0) {
        buffer = new int[_precise_reads];
        if(buffer == NULL) {
            ulog(F("Could not allocate precise buffer."));
        }
        else {
            buffer_reset();
            _started = true; // TODO: should that be moved to derived functions?
        }
    } else {
        _started = true;
    }
}

bool Input_Base::measure() {  
//    if(_num_mux_pins > 0) {
//            if(i>0) value().add(F(","));
// }
    int val = read();
//    value().add(val);
    value().from(val);
    return true;
}

void Input_Base::buffer_reset() {
    buffer_fill = 0;
    // switch to next mux
    if(_num_mux_pins > 0) {
        _current_mux = (_current_mux+1) % (1<<_num_mux_pins);
        for (int p = 0; p < _num_mux_pins; p++) {
            digitalWrite(_mux_pins[p], _current_mux & (1<<p) ? HIGH : LOW);
        }
        // delayMicroseconds(2); // let value stabilize - TODO: think if necessary
    }
}

Input_Base& Input_Base::_mux(int num_pins, ...) {
    _num_mux_pins = num_pins;

    va_list args;
    va_start(args, num_pins);

    for (int i = 0; i < num_pins; i++) {
        _mux_pins[i] = va_arg(args, int);
    }

    va_end(args);

    return *this;
}
