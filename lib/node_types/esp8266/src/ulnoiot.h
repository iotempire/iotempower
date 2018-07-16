// all we need to address arduino-like stuff
#ifndef _ULNOIOT_H_
#define _ULNOIOT_H_

#include <Arduino.h>
#include "ulnoiot-default.h"
#include "config.h"
#include "pins.h"
#include "devices.h"

// allow using more familiar naming scheme for setup and loop
#define setup() ulnoiot_setup()
//#define loop() ulnoiot_loop()

#endif // _ULNOIOT_H_