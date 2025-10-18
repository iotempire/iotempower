/* setup_includer
 * author: ulno
 * creation: 2018-09-03
 * 
 * This file helps including the user's setup.cpp code.
 * */

#include <iotempower.h>

void (iotempower_start)() __attribute__((weak));
void (iotempower_platform_early_init)() __attribute__((weak));

#define init() \
     iotempower_init()

#define start() \
    iotempower_start()




/**
 * AUTOMATIC SLEEP MANAGER
 * ========================
 * Create a global sleep manager instance available to all ESP nodes
 * This provides sleep management functionality without requiring explicit user declaration
 */
sleep_mgr(sleep_mgr);

// That's the user file (called in folder setup.cpp, but linked as setup.h
// here)
#include "setup.h"

#undef start
#undef init