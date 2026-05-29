// virtual.h
// virtual device class (custom MQTT commands, status updates, and timed loop)

#ifndef _VIRTUAL_H_
#define _VIRTUAL_H_

#include <device.h>

#define VIRTUAL_COMMAND_HANDLER std::function<void(Ustring& command)>
#define VIRTUAL_LOOP_HANDLER std::function<void()>

typedef struct {
    const char *key;
    VIRTUAL_COMMAND_HANDLER method;
} Virtual_Command_Method;

class Virtual_Device : public Device {
    private:
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame = millis();
        VIRTUAL_LOOP_HANDLER _loop_handler = NULL;
        Virtual_Command_Method command_methods[IOTEMPOWER_MAX_ANIMATOR_COMMANDS];
        int command_method_counter = 0;
        bool _value_updated = false;

        void init();

    public:
        Virtual_Device(const char* name) : Device(name, 0) {
            init();
        }

        Virtual_Device& with_loop(VIRTUAL_LOOP_HANDLER handler) {
            _loop_handler = handler;
            return *this;
        }

        // Alias for animator-like API naming
        Virtual_Device& with_frame_builder(VIRTUAL_LOOP_HANDLER handler) {
            return with_loop(handler);
        }

        // adds so that first matched will be called "" matches always and should be added last
        Virtual_Device& with_command_handler(const char* command_name, VIRTUAL_COMMAND_HANDLER handler);
        Virtual_Device& with_command_handler(VIRTUAL_COMMAND_HANDLER handler) {
            return with_command_handler("", handler);
        }

        void set_fps(int fps) {
            if(fps < 1) fps = 1;
            if(fps > 100) fps = 100;
            _fps = fps;
            frame_len = 1000 / fps;
        }

        Virtual_Device& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }

        Virtual_Device& fps(int fps) {
            set_fps(fps);
            return *this;
        }

        int get_fps() {
            return _fps;
        }

        Virtual_Device& report(const char* status) {
            value().from(status);
            _value_updated = true;
            return *this;
        }

        Virtual_Device& report(const Ustring& status) {
            value().copy(status);
            _value_updated = true;
            return *this;
        }

        Virtual_Device& report(long status) {
            value().from(status);
            _value_updated = true;
            return *this;
        }

        Virtual_Device& report(unsigned long status) {
            value().from(status);
            _value_updated = true;
            return *this;
        }

        Virtual_Device& report(int status) {
            value().from(status);
            _value_updated = true;
            return *this;
        }

        void start() {
            _started = true;
        }

        virtual bool measure();
};

#endif // _VIRTUAL_H_
