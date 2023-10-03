// devices.h
// list of all available device creation functions

// TODO: think how to make includes conditional, reducing compiletime

#ifndef _DEVICES_H_
#define _DEVICES_H_


#include <device.h>
#include <device-manager.h>

// use init_priority to make sure these get initialized last
#define IOTEMPOWER_DEVICE_(Class_Name, internal_name, gcc_va_args...) \
    Class_Name iotempower_dev_ ## internal_name __attribute__((init_priority(65535))) \
        = Class_Name(gcc_va_args); \
    Class_Name& internal_name = (Class_Name&) iotempower_dev_ ## internal_name


// convenience macros for device definition
// Internal name (IN) references the internal name created with DN
#define IN(name) iotempower_dev_##name

#define IOTEMPOWER_DEVICE(name, macro_name, ...) macro_name(iotempower_dev_##name, #name , ##__VA_ARGS__)

#include "devices_generated.h"

#endif // _DEVICES_H_

