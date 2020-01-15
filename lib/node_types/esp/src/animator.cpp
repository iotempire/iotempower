// animator.cpp
// animator class (control frame-based animations)

#include <toolbox.h>
#include "animator.h"

void Animator::init() {
    add_subdevice(new Subdevice("",true))->with_receive_cb(
        [&] (const Ustring& payload) {
            value().copy(payload); // save it
            // search for command
            for(int i=0; i<command_method_counter; i++) {
                const char* key = command_methods[i].key;
                ulog(F("Trying %s"),key);
                int len = strlen(key);
                if(payload.starts_with(key) && payload.as_cstr()[len] <= 32) {
                    Ustring command(payload.as_cstr()+len);
                    command.strip();
                    command_methods[i].method(command);
                    break;
                }
            }
            return true;
        }
    );
    set_fps(24);
}

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

Animator& Animator::with_command_handler(const char* command_name, ANIMATOR_COMMAND_HANDLER handler) {
    if(command_method_counter<IOTEMPOWER_MAX_ANIMATOR_COMMANDS) {
        command_methods[command_method_counter].key = command_name;
        command_methods[command_method_counter].method = handler;
        command_method_counter++;
    } else {
        ulog(F("Too many commands for animator given."));
    }
    return *this;
}

