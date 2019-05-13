/* setup_includer
 * author: ulno
 * creation: 2018-09-03
 * 
 * This file helps including the user's setup.cpp code.
 * */

#include <iotempower.h>

void (ulnoiot_init)() __attribute__((weak));
void (ulnoiot_start)() __attribute__((weak));

#define init() \
    ulnoiot_start()

#define start() \
    ulnoiot_start()

// That's the user file (called in folder setup.cpp, but linked as setup.h
// here)
#include "setup.h"
