// hcsr04.cpp

// Important to include Functional Interrupt as else it does not allow to 
// define a class-based (object specific) interrupt
#include <FunctionalInterrupt.h>
#include "distance-hcsr04.h"

void Hcsr04::echo_changed(void) {
    unsigned long current = micros();  // directly read time to be as precise as possible
    if(triggered) {
        if(digitalRead(_echo_pin)) { // this went up -> measure burst start
            interval_start = current;
        } else { // is down, so that is the measure burst end
            interval_end = current;
            triggered = false; // prevent another trigger
            measured = true;
        }
    }
}

Hcsr04::Hcsr04(const char* name, uint8_t trigger_pin, uint8_t echo_pin)
    : Device(name, 10000) {
    
    _timeout_us = timeout_us;
    _echo_pin = echo_pin;
    _trigger_pin = trigger_pin;
    add_subdevice(new Subdevice(""));
}

void Hcsr04::start() {
    pinMode(_echo_pin, INPUT); // no pullup
    pinMode(_trigger_pin, OUTPUT);
    digitalWrite(_trigger_pin, 0);
    last_triggered = micros()-(IOTEMPOWER_HCSR04_INTERVAL*1000);
    delay(1);  // let port settle
    attachInterrupt(digitalPinToInterrupt(_echo_pin),
         [&] () { echo_changed(); }, CHANGE); 
    _started = true;
}


bool Hcsr04::measure() {
    unsigned long current = micros();
    unsigned long delta = current - last_triggered;

    // triggering means to pull up the trigger pin for at least 10ms
    // this triggers an echo to be sent out
    // when the echo is started to be sent out the echo pin is pulled
    // up by the hcsr04 until it receives the echo back then that is pulled
    // down.
    if(triggered) {
        if(delta >= _timeout_us) { // timed out -> reset
            ulog(F("hcrs04 timeout %lu"), current);
            digitalWrite(_trigger_pin, 0); // set pin back to 0 (should already be)
            trigger_started = false;
            triggered = false;
            measured = false;
        }
    } else { // not triggered -> trigger new
        // Eventually trigger new signal
        if(!trigger_started) {
            if(delta >= IOTEMPOWER_HCSR04_INTERVAL*1000) {
                last_triggered = current;
                digitalWrite(_trigger_pin,1); // start signal
                trigger_started = true;
            } 
        } else { // trigger was started
            // First we need to send a 10ms signal
            if(delta >= 10000) { // this should be done now
                digitalWrite(_trigger_pin, 0); // set pin back to 0
                triggered = true;
                trigger_started = false;
            }
        }
    }
    if(measured) {
        delta = interval_end - interval_start;
        measured = false; // release lock
        unsigned int d = (unsigned int)(delta*100/582);
        if(d<=IOTEMPOWER_HCSR04_MAX_DISTANCE) {
            if(distance_buffer_fill < IOTEMPOWER_HCSR04_BUFFER_SIZE) {
                // let buffer fill first
                distance_buffer[distance_buffer_fill] = d;
                distance_sum += d;
                distance_buffer_fill ++;
            } else { // we start returning values, when the buffer is full
                // add new distance to buffer start
                distance_sum -= distance_buffer[IOTEMPOWER_HCSR04_BUFFER_SIZE-1]; // this is replaced
                for(int i=IOTEMPOWER_HCSR04_BUFFER_SIZE-1; i>=1; i--) {
                    distance_buffer[i] = distance_buffer[i-1];
                }
                distance_buffer[0] = d;
                distance_sum += d;
                // do some median/average thing over buffer (discard values that are too far off from average)
                // unsigned long valid_sum = 0;
                // unsigned int valid_count = 0;
                // for(int i=IOTEMPOWER_HCSR04_BUFFER_SIZE-1; i>=0; i--) {
                //     unsigned int d = distance_buffer[i];
                //     if(abs(d * IOTEMPOWER_HCSR04_BUFFER_SIZE - distance_sum)
                //         <= IOTEMPOWER_HCSR04_BUFFER_SIZE*IOTEMPOWER_HCSR04_MAX_DISTANCE / 5) { // less than 20% derivation from average
                //         valid_sum += d;
                //         valid_count ++; 
                //     }
                // }
                // if(valid_count > IOTEMPOWER_HCSR04_BUFFER_SIZE/2) {
                // d = (valid_sum + valid_count/2) / valid_count;
                
                // real median
                unsigned int median_buffer[IOTEMPOWER_HCSR04_BUFFER_SIZE];
                for(int i=0; i<IOTEMPOWER_HCSR04_BUFFER_SIZE; i++) {
                    int j;
                    for(j=i; j>0; j--) { // swap it down to right position
                        if(distance_buffer[i]<median_buffer[j-1])
                            median_buffer[j] = median_buffer[j-1]; // make space and move up
                        else break;
                    }
                    median_buffer[j]=distance_buffer[i]; // add at right position
                }
                d = median_buffer[IOTEMPOWER_HCSR04_BUFFER_SIZE/2]; // TODO: consider doing median only via filter
                value().from(d);
                return true;
            } // endif buffer is filled
        } // endif under max distance
    }
    return false; // no new value available
}
