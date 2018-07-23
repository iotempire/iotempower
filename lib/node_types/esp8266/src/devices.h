// devices.h
// list of all available device creation functions

#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "device.h"
#include "device-manager.h"

bool add_device(Device& device);

#include "output.h"
Output& output(const char* name, const int pinnr, const char* high, const char* low) {
    Output* dev = new Output(name, pinnr, high, low);
    if(dev != NULL) {
        if(!add_device(*dev)) {
            delete dev;
            dev = NULL; // will make program crash
        }
    }
    // TODO: consider clean exception handling
    return *dev; // will crash the program, if something here went wrong
}
Output& output(const char* name, int pinnr) {
    return output(name, pinnr, "on", "off");
}

#include "input.h"
// can also do with_pullup and with_threshold
Input& input(const char* name, int pinnr, const char* high, const char* low) {
    Input* dev = new Input(name, pinnr, high, low);
    if(dev != NULL) {
        if(!add_device(*dev)) {
            delete dev;
            dev = NULL; // will make program crash
        }
    }
    // TODO: consider clean exception handling
    return *dev; // will crash the program, if something here went wrong
}
Input& input(const char* name, int pinnr) {
    return input(name, pinnr, "on", "off");
}

#endif // _DEVICES_H_