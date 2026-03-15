#ifndef _PLATFORM_EXTRAS_H_
#define _PLATFORM_EXTRAS_H_

#include <M5StickCPlus2.h>
#include "toolbox.h"

/**
 * @brief M5StickC Plus2 specific early initialization
 * 
 * CRITICAL: This function MUST be called at the very beginning of setup()
 *         to ensure that the M5StickC Plus2 is properly powered on.
 */
inline void iotempower_platform_early_init() {
    // Initialize M5 system first, to make sure power is held
    auto cfg = M5.config();
    StickCP2.begin(cfg);
}

#endif // _PLATFORM_EXTRAS_H_
