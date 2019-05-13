// all we need to address arduino-like stuff
#ifndef _IOTEMPOWER_H_
#define _IOTEMPOWER_H_

#include <Arduino.h>
#include "iotempower-default.h"
#include "config.h"
#include "device.h"
#include "device-manager.h"
#include "toolbox.h"
#include "devices.h"

// needs to be last
#include "pins-wrapper.h"

/* Set the amount of seconds to pass before doing full status report.
 * If interval=0, only report changes.
 *  */
bool transmission_interval(int interval);

#endif // _IOTEMPOWER_H_