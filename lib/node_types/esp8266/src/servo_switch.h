// servo_switch.h
// control servo motor as a switch


#ifndef _SERVO_SWITCH_H_
#define _SERVO_SWITCH_H_

#include <servo.h>

class Servo_Switch : public Servo {
    private:
        int _on_angle;
        int _off_angle;
        int _return_angle;
        const char* _on_command;
        const char* _off_command;
        bool needs_return=false;
        void init(int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command);
    protected:
        virtual void do_register();
    public:
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us, int duration)
                : Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, return_angle, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us, int duration)
                : Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, -1, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us)
                : Servo(name, pin, min_us, max_us) {
            init(on_angle, off_angle, return_angle, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                int min_us, int max_us, int duration)
                : Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, return_angle, "on", "off");
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                int min_us, int max_us)
                : Servo(name, pin, min_us, max_us) {
            init(on_angle, off_angle, return_angle, "on", "off");
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle)
                : Servo(name, pin) {
            init(on_angle, off_angle, return_angle, "on", "off");
        }
        Servo_Switch(const char* name, uint8_t pin)
                : Servo(name, pin) {
            init(0, 180, 90, "on", "off");
        }
        void set(const Ustring& status);
        bool measure(void);
        // void start() //handled in base class
};


#endif // _SERVO_H_