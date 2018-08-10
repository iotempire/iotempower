// animator.h
// animator class (control frame-based animations)

#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <device.h>
// TODO: should this dependencies softened to classes having a show-method?
#include <rgb_matrix.h>


class Animator : public Device {
    private:
        int _fps;
        unsigned long frame_len;
        unsigned long last_frame=millis();
        #define ANIMATOR_FRAME_BUILDER_HANDLER std::function<void()>
        ANIMATOR_FRAME_BUILDER_HANDLER _handler = NULL;
        #define ANIMATOR_COMMAND_HANDLER std::function<void(const Ustring& command)>
        ANIMATOR_COMMAND_HANDLER _command_handler = NULL;
        #define ANIMATOR_SHOW std::function<void()>
        ANIMATOR_SHOW _show_handler = NULL;

        void init() {
            add_subdevice(new Subdevice(""));
            add_subdevice(new Subdevice("set",true))->with_receive_cb(
                [&] (const Ustring& payload) {
                    measured_value(1).from(payload); // save it
                    if(_command_handler) _command_handler(payload);
                    return true;
                }
            );
            set_fps(24);
        }
        RGB_Matrix* _matrix = NULL;
    public:
        Animator(const char* name) : Device(name) {
            init();            
        }
        Animator& with_frame_builder(ANIMATOR_FRAME_BUILDER_HANDLER handler) {
            _handler = handler;
            return *this;
        }
        Animator& with_show(ANIMATOR_SHOW handler) {
            _show_handler = handler;
            return *this;
        }
        Animator(const char* name, RGB_Matrix& matrix) : Device(name) {
            init();
            _matrix = &matrix;
            with_show([&] () { _matrix->show(); });
        }
        Animator& with_command_handler(ANIMATOR_COMMAND_HANDLER handler) {
            _command_handler = handler;
            return *this;
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
        void show() {
            if(_show_handler) _show_handler();
        }
        bool measure();
};


#endif // _ANIMATOR_H_