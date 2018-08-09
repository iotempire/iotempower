// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <device.h>
#include <device-manager.h>

bool add_device(Device& device);

#define CHECK_DEV(dev) \
    if((dev) != NULL) { \
        if(!add_device(*(dev))) { \
            delete (dev); \
            (dev) = NULL; \
            log("Couldn't allocate new device."); \
        } \
    } \
    return *(dev);


#include <output.h>
Output& output(const char* name, const int pinnr, 
        const char* high="on", const char* low="off") {
    Output* dev = new Output(name, pinnr, high, low);
    CHECK_DEV(dev)
}

#include <input.h>
// can also do with_pullup and with_threshold
Input& input(const char* name, int pinnr, 
        const char* high="on", const char* low="off") {
    Input* dev = new Input(name, pinnr, high, low);
    CHECK_DEV(dev)
}

#include <analog.h>
Analog& analog(const char* name) {
    Analog* dev = new Analog(name);
    CHECK_DEV(dev)
}

#include <th.h>
Dht11& dht11(const char* name, int pinnr) {
    Dht11* dev = new Dht11(name, pinnr);
    CHECK_DEV(dev)
}

Ds18b20& ds18b20(const char* name, int pinnr) {
    Ds18b20* dev = new Ds18b20(name, pinnr);
    CHECK_DEV(dev)
}

#include <rgb_single.h>
RGB_Single& rgb(const char* name, int pin_r, int pin_g, int pin_b, 
        bool invert_signal=false) {
    RGB_Single* dev = new RGB_Single(name, pin_r, pin_g, pin_b, invert_signal);
    CHECK_DEV(dev)
}

#include <animator.h>
Animator& animator(const char* name) {
    Animator* dev = new Animator(name);
    CHECK_DEV(dev)
}

// TODO: fix error checking in this module, this can crash horribly if there is no
// memory left
#define rgb_strip(name, led_count, type, ...) \
    CREATE_RGB_STRIP(name, led_count, type, __VA_ARGS__)


#include <rgb_matrix.h>
RGB_Matrix& rgb_matrix(const char* name, int width, int height) {
    RGB_Matrix* dev = new RGB_Matrix(name, width, height);
    CHECK_DEV(dev)
}
RGB_Matrix& rgb_matrix(const char* name, RGB_Base& strip) {
    RGB_Matrix* dev = new RGB_Matrix(name, strip);
    CHECK_DEV(dev)
}


#endif // _DEVICES_H_