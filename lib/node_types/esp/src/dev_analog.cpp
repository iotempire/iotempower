// analog.cc
#include "dev_analog.h"

static unsigned long last_read = millis()-5;

bool Analog::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 3) return false;
    last_read = current_time;
    if(started()) { 
        if(_num_mux_pins > 0) {
            int v = 0;
            value().clear();
            for( int i=0; i<(1<<_num_mux_pins); i++ ) {
                for (int p = 0; p < _num_mux_pins; p++) {
                    digitalWrite(_mux_pins[p], i & (1<<p) ? HIGH : LOW);
                }
                yield(); // allow multitasking
                delayMicroseconds(2); // let value stabilize
                if(i>0) value().add(F(","));
                value().add(analogRead(_pin));
            }
            return true;
        } else {
            value().from(analogRead(_pin));
            return true;
        }
    }
    return false; // not started
}


void Analog::start() {
    // TODO: check on esp32 if Analog pin needs to be set up
    if(_num_mux_pins > 0) {
        for (int i = 0; i < _num_mux_pins; i++) {
            pinMode(_mux_pins[i], OUTPUT);
            digitalWrite(_mux_pins[i], LOW);
        }
    }
    _started = true;
    measure();
}

Analog& Analog::_mux(int num_pins, ...) {
    _num_mux_pins = num_pins;

    va_list args;
    va_start(args, num_pins);

    for (int i = 0; i < num_pins; i++) {
        _mux_pins[i] = va_arg(args, int);
    }

    va_end(args);

    return *this;
}
