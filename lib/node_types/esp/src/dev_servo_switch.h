// servo_switch.h
// control servo motor as a switch


#ifndef _SERVO_SWITCH_H_
#define _SERVO_SWITCH_H_

#include <emp_servo.h>

class Servo_Switch : public Emp_Servo {
    private:
        int _on_angle;
        int _off_angle;
        int _return_angle;
        const char* _on_command;
        const char* _off_command;
        bool needs_return=false;
        void init(int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command);
    public:
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us, int duration)
                : Emp_Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, return_angle, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us, int duration)
                : Emp_Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, -1, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                const char* on_command, const char* off_command,
                int min_us, int max_us)
                : Emp_Servo(name, pin, min_us, max_us) {
            init(on_angle, off_angle, return_angle, on_command, off_command);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                int min_us, int max_us, int duration)
                : Emp_Servo(name, pin, min_us, max_us, duration) {
            init(on_angle, off_angle, return_angle, str_on, str_off);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle,
                int min_us, int max_us)
                : Emp_Servo(name, pin, min_us, max_us) {
            init(on_angle, off_angle, return_angle, str_on, str_off);
        }
        Servo_Switch(const char* name, uint8_t pin,
                int on_angle, int off_angle, int return_angle)
                : Emp_Servo(name, pin) {
            init(on_angle, off_angle, return_angle, str_on, str_off);
        }
        Servo_Switch(const char* name, uint8_t pin)
                : Emp_Servo(name, pin) {
            init(0, 180, 90, str_on, str_off);
        }
        Servo_Switch& set(const Ustring& status);
        void process(const Ustring& value);
        bool measure(void);
        // void start() //handled in base class
};


#endif // _SERVO_H_