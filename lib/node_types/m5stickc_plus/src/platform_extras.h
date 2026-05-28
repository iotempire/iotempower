#ifndef _PLATFORM_EXTRAS_H_
#define _PLATFORM_EXTRAS_H_

#include <M5Unified.h>
#include "toolbox.h"

// M5StickC Plus compatibility layer:
// Map the Plus2-style StickCP2 singleton to the M5Unified M5 object so that
// device source files shared between Plus and Plus2 can use StickCP2.* uniformly.
#define StickCP2 M5

/**
 * @brief M5StickC Plus specific early initialization
 *
 * CRITICAL: This function MUST be called at the very beginning of setup()
 *           to ensure that the M5StickC Plus is properly powered on.
 */
inline void iotempower_platform_early_init() {
    // Initialize M5 system first, to make sure power is held
    auto cfg = M5.config();
    M5.begin(cfg);
}

#endif // _PLATFORM_EXTRAS_H_
