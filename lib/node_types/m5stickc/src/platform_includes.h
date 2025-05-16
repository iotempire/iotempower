#ifndef _PLATFORM_INCLUDES_H_
#define _PLATFORM_INCLUDES_H_

#ifdef ENV_M5STICKC
    #include <M5StickC.h>
#elif ENV_M5STICKC_PLUS
    #include <M5StickCPlus.h>
#elif ENV_M5STICKC_PLUS2
    #include <M5StickCPlus2.h>
#endif

#endif // _PLATFORM_INCLUDES_H_