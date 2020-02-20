// animator.h
// animator class (control frame-based animations)

#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <device.h>
// TODO: should these dependencies softened to classes having a show-method?
#include <rgb_matrix.h>

#define ANIMATOR_COMMAND_HANDLER std::function<void(Ustring& command)>
typedef struct {
    const char *key;
    ANIMATOR_COMMAND_HANDLER method;
} _COMMAND_METHOD;

// TODO: think if this should become a singleton
class Animator : public Device {
    private:
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        #define ANIMATOR_FRAME_BUILDER_HANDLER std::function<void()>
        ANIMATOR_FRAME_BUILDER_HANDLER _handler = NULL;
        _COMMAND_METHOD command_methods[IOTEMPOWER_MAX_ANIMATOR_COMMANDS];
        int command_method_counter=0;
        #define ANIMATOR_SHOW std::function<void()>
        ANIMATOR_SHOW _show_handler = NULL;
        RGB_Matrix* _matrix = NULL;

        void init();

    public:
        Animator(const char* name) : Device(name, 0) {
            init();
            ulog("animator init pollrate: %lu", get_pollrate_us());
        }
        Animator& with_frame_builder(ANIMATOR_FRAME_BUILDER_HANDLER handler) {
            _handler = handler;
            return *this;
        }
        Animator& with_show(ANIMATOR_SHOW handler) {
            _show_handler = handler;
            return *this;
        }
        Animator(const char* name, RGB_Matrix& matrix) : Device(name, 0) {
            init();
            _matrix = &matrix;
            with_show([&] () { _matrix->show(); });
        }
        // adds so that first matched will be called "" matches always and should be added last
        Animator& with_command_handler(const char* command_name, ANIMATOR_COMMAND_HANDLER handler);
        Animator& with_command_handler(ANIMATOR_COMMAND_HANDLER handler) {
            return with_command_handler("", handler);
        }
        void set_fps(int fps) {
            if(fps<1) fps=1;
            if(fps>100) fps=100;
            _fps = fps;
            frame_len = 1000 / fps;
        }
        Animator& with_fps(int fps) {
            set_fps(fps);
            return *this;
        }
        Animator& fps(int fps) {
            set_fps(fps);
            return *this;
        }
        int get_fps() {
            return _fps;
        }

        void show() {
            if(_show_handler) _show_handler();
        }
        void start() {
            // does nothing physical
            _started = true;
        }
        virtual bool measure();
};


#endif // _ANIMATOR_H_