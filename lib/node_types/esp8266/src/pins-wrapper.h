// load modified pins

#ifndef _IOTEMPOWER_PINS_H_
#define _IOTEMPOWER_PINS_H_

#include <pins.h>

#ifdef ONBOARDLED

    #ifdef LED_BUILTIN
        #undef LED_BUILTIN
    #endif
    #define LED_BUILTIN ONBOARDLED

#endif // ONBOARDLED


#ifndef FLASHBUTTON

    #ifdef D3
        #define FLASHBUTTON D3
    #else
        #define FLASHBUTTON 0
    #endif // D3

#endif // FLASHBUTTON


#endif // _IOTEMPOWER_PINS_H_
