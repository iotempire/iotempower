// uservo.h
// control servo motor
// (u-servo as servo is used by arduino servo class)


#ifndef _SERVO_H_
#define _SERVO_H_

#include <_Servo.h>
#include <device.h>

#define ULNOIOT_DEFAULT_DURATION 700

class Servo : public Device {
    private:
        uint8_t _pin;
        int _min_us;
        int _max_us;
        _Servo _servo;
        int _value;
        bool stopped = true;
        int _duration;
        unsigned long start_time;
        void init(uint8_t pin, int min_us, int max_us, int duration);
    public:
        Servo(const char* name, uint8_t pin, int min_us, int max_us)
            : Device(name) {
            init(pin, min_us, max_us, ULNOIOT_DEFAULT_DURATION);
        }
        Servo(const char* name, uint8_t pin, int min_us, int max_us, int duration)
            : Device(name) {
            init(pin, min_us, max_us, duration);
        }
        Servo(const char* name, uint8_t pin) : Device (name) {
            init(pin, 554, 2400, ULNOIOT_DEFAULT_DURATION);
        }
        Servo(const char* name, uint8_t pin, int duration) : Device (name) {
            init(pin, 554, 2400, duration);
        }
        void stop();
        void set(int value);
        bool measure(void);
};


#endif // _SERVO_H_