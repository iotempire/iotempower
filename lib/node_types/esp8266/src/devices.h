// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "device.h"
#include "device-manager.h"

bool add_device(Device& device);

#define CHECK_DEV(dev) \
    if((dev) != NULL) { \
        if(!add_device(*(dev))) { \
            delete (dev); \
            (dev) = NULL; \
            controlled_crash("Couldn't allocate new device."); \
        } \
    } \
    return *(dev);


#include "output.h"
Output& output(const char* name, const int pinnr, const char* high, const char* low) {
    Output* dev = new Output(name, pinnr, high, low);
    CHECK_DEV(dev)
}
Output& output(const char* name, int pinnr) {
    return output(name, pinnr, "on", "off");
}

#include "input.h"
// can also do with_pullup and with_threshold
Input& input(const char* name, int pinnr, const char* high, const char* low) {
    Input* dev = new Input(name, pinnr, high, low);
    CHECK_DEV(dev)
}
Input& input(const char* name, int pinnr) {
    return input(name, pinnr, "on", "off");
}

#include "analog.h"
Analog& analog(const char* name) {
    Analog* dev = new Analog(name);
    CHECK_DEV(dev)
}

#include "th.h"
Dht11& dht11(const char* name, int pinnr) {
    Dht11* dev = new Dht11(name, pinnr);
    CHECK_DEV(dev)
}

Ds18b20& ds18b20(const char* name, int pinnr) {
    Ds18b20* dev = new Ds18b20(name, pinnr);
    CHECK_DEV(dev)
}

#include "rgb_single.h"
RGB_Single& rgb(const char* name, int pin_r, int pin_g, int pin_b, bool invert_signal) {
    RGB_Single* dev = new RGB_Single(name, pin_r, pin_g, pin_b, invert_signal);
    CHECK_DEV(dev)
}

RGB_Single& rgb(const char* name, int pin_r, int pin_g, int pin_b) {
    RGB_Single* dev = new RGB_Single(name, pin_r, pin_g, pin_b);
    CHECK_DEV(dev)
}

#include "rgb_multi.h"
template<uint8_t pin, int led_count> RGB_Multi<pin, led_count>& _rgb_multi(const char* name) {
    RGB_Multi<pin, led_count>* dev = new RGB_Multi<pin, led_count>(name);
    CHECK_DEV(dev)
}
#define rgb_multi(name, pin, led_count) _rgb_multi<pin, led_count>(name)

#endif // _DEVICES_H_