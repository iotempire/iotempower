// animator.cpp
// animator class (control frame-based animations)

#include <toolbox.h>
#include "animator.h"


// measure calls show in the end, so no show in graphics functions.
bool Animator::measure() {
    if(_handler) {
        unsigned long current = millis();
        int count=0;
        bool changed=false;
        while(current - last_frame >= frame_len) {
            _handler();
            changed = true;
            last_frame += frame_len;
            count ++;
            if( count>=3 ) {
                last_frame = current;
                break; // only replay max 3 frames
            }
        }
        if(changed) show();
    }
    return false; // no values are generated here TODO: or true to return last command or frame number?
}
