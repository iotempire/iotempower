// servo.h
// control servo motor


#ifndef _SERVO_H_
#define _SERVO_H_

#include <_Servo.h>
#include <device.h>

#define IOTEMPOWER_DEFAULT_DURATION 700

class Servo : public Device {
    private:
        uint8_t _pin;
        int _min_us;
        int _max_us;
        _Servo _servo;
        int _value;
        bool stopped = true;
        void init(uint8_t pin, int min_us, int max_us, int duration);
        void do_register();
    protected:
        int _duration;
        unsigned long start_time;
    public:
        Servo(const char* name, uint8_t pin, int min_us, int max_us)
            : Device(name) {
            init(pin, min_us, max_us, IOTEMPOWER_DEFAULT_DURATION);
        }
        Servo(const char* name, uint8_t pin, int min_us, int max_us, int duration)
            : Device(name) {
            init(pin, min_us, max_us, duration);
        }
        Servo(const char* name, uint8_t pin) : Device (name) {
            init(pin, 554, 2400, IOTEMPOWER_DEFAULT_DURATION);
        }
        Servo(const char* name, uint8_t pin, int duration) : Device (name) {
            init(pin, 554, 2400, duration);
        }
        void stop();
        Servo& turn_to(int value);
        Servo& set(int value);
        virtual void process(const Ustring& value);
        virtual bool measure(void);
        void start();
};


#endif // _SERVO_H_