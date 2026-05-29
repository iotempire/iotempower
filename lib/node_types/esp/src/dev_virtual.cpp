// virtual.cpp
// virtual device class (custom MQTT commands, status updates, and timed loop)

#include <toolbox.h>
#include "dev_virtual.h"

void Virtual_Device::init() {
    add_subdevice(new Subdevice("", true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // search for command
            for(int i = 0; i < command_method_counter; i++) {
                const char* key = command_methods[i].key;
                int len = strlen(key);
                if(payload.starts_with(key) && payload.as_cstr()[len] <= 32) {
                    Ustring command(payload.as_cstr() + len);
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

bool Virtual_Device::measure() {
    bool changed = _value_updated;

    if(_loop_handler) {
        unsigned long current = millis();
        int count = 0;
        while(current - last_frame >= frame_len) {
            _loop_handler();
            changed |= _value_updated;
            last_frame += frame_len;
            count++;
            if(count >= 3) {
                last_frame = current;
                break; // only replay max 3 frames
            }
        }
    }

    _value_updated = false;
    return changed;
}

Virtual_Device& Virtual_Device::with_command_handler(const char* command_name, VIRTUAL_COMMAND_HANDLER handler) {
    if(command_method_counter < IOTEMPOWER_MAX_ANIMATOR_COMMANDS) {
        command_methods[command_method_counter].key = command_name;
        command_methods[command_method_counter].method = handler;
        command_method_counter++;
    } else {
        ulog(F("Too many commands for virtual device given."));
    }
    return *this;
}
