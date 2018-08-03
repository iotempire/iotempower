// all we need to address arduino-like stuff
#ifndef _ULNOIOT_H_
#define _ULNOIOT_H_

#include <Arduino.h>
#include "ulnoiot-default.h"
#include "config.h"
#include "device.h"
#include "device-manager.h"
#include "toolbox.h"
#include "devices.h"

// needs to be last
#include "pins.h"

// allow using more familiar naming scheme for setup and loop
#define setup() ulnoiot_setup()
//#define loop() ulnoiot_loop()

/* Set the amount of seconds to pass before doing full status report.
 * If interval=0, only report changes.
 *  */
bool transmission_interval(int interval);

#endif // _ULNOIOT_H_