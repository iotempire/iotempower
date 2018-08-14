// trigger.cc
#include "trigger.h"

// brainfuck for messed-up attachInterrupt design in Arduino code
// Assume 16 pins
#define ULNOIOT_MAX_INTERRUPT_COUNTERS 16
static unsigned long counters[ULNOIOT_MAX_INTERRUPT_COUNTERS] = 
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
template<uint8_t PIN> void static bf_counter() {
    //ulog("bf_counter %d",PIN);
    if(counters[PIN] < ULNOIOT_MAX_TRIGGER)
        counters[PIN]++;
    else counters[PIN] = 0;
}
typedef void (*ulnoiot_voidFuncPtr)(void);

static ulnoiot_voidFuncPtr bf_counters[ULNOIOT_MAX_INTERRUPT_COUNTERS] =
{
    bf_counter<0>,
    bf_counter<1>,
    bf_counter<2>,
    bf_counter<3>,
    bf_counter<4>,
    bf_counter<5>,
    bf_counter<6>,
    bf_counter<7>,
    bf_counter<8>,
    bf_counter<9>,
    bf_counter<10>,
    bf_counter<11>,
    bf_counter<12>,
    bf_counter<13>,
    bf_counter<14>,
    bf_counter<15>,
};

void Trigger::reinit() {
    detachInterrupt(_pin);
    if(_pull_up) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }
    int mode;
    if(_falling) {
        if(_rising) mode = CHANGE;
        else mode = FALLING;
    } else { // not falling
        if(_rising) mode = RISING;
        else mode = 0;
    }
    // This must use a static function???!!!! (who designed this?!)
    // Using some brainfuck to make this object oriented
    attachInterrupt(digitalPinToInterrupt(_pin), bf_counters[_pin], mode); 
    measure();
}

bool Trigger::measure() {
    bool measured = false;
    noInterrupts();
    if(last_counter != counters[_pin]) {
        last_counter = counters[_pin];
        measured = true;
        measured_value().from(last_counter);
    }
    interrupts();
    return measured;
}
