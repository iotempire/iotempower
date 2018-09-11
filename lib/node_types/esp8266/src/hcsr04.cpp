// hcsr04.cpp

// Important to include Functional Interrupt as else it does not allow to 
// define a class-based (object specific) interrupt
#include <FunctionalInterrupt.h>
#include "hcsr04.h"


void Hcsr04::echo_changed(void) {
    unsigned long current = micros();
    if(triggered) {
        if(digitalRead(_echo_pin)) { // this went up -> burst start
            interval_start = current;
        } else { // is down, so that is the ned
            interval_end = current;
            triggered = false; // prevent another trigger
            measured = true;
        }
    }
}

Hcsr04::Hcsr04(const char* name, uint8_t trigger_pin, uint8_t echo_pin, 
    unsigned long timeout_us)
    : Device(name) {
    
    _timeout_us = timeout_us;
    _echo_pin = echo_pin;
    _trigger_pin = trigger_pin;
    add_subdevice(new Subdevice(""));
}

void Hcsr04::start() {
    pinMode(_echo_pin, INPUT); // no pullup
    pinMode(_trigger_pin, OUTPUT);
    digitalWrite(_trigger_pin, 0);
    last_triggered = micros()-(ULNOIOT_HCSR04_INTERVAL*1000);
    delay(1);  // let port settle
    attachInterrupt(digitalPinToInterrupt(_echo_pin),
         [&] () { echo_changed(); }, CHANGE); 
    _started = true;
}


bool Hcsr04::measure() {
    unsigned long current = micros();

    // Eventually trigger new signal
    unsigned long delta = current - last_triggered;
    if(delta >= ULNOIOT_HCSR04_INTERVAL*1000) {
        last_triggered = micros();
        digitalWrite(_trigger_pin,1);
    } else {
        // First we need to send a 10ms signal
        if(delta >= 10000) { // this shoudl be done now
            digitalWrite(_trigger_pin, 0); // set pin back to 0
            triggered = true;
        }
    }
    if(measured) {
        measured = false;
        delta = interval_end - interval_start;
        if(delta < _timeout_us) { // seems valid, compute and save distance
            unsigned long d = delta*100/582;
            if(measured_value().empty() 
                    || abs(measured_value().as_int() - d) >= _precision ) {
                measured_value().from(d);
                return true;
            }
        }
    }
    return false; // no new value available
}
