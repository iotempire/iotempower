// emp_servo.h
// control servo motor


#ifndef _EMP_SERVO_H_
#define _EMP_SERVO_H_

#include <_Servo.h>
#include <device.h>

#define IOTEMPOWER_SERVO_DEFAULT_DURATION 700
#define IOTEMPIRE_SERVO_POLLRATE 1000

class Emp_Servo : public Device {
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
        Emp_Servo(const char* name, uint8_t pin, int min_us, int max_us)
            : Device(name, IOTEMPIRE_SERVO_POLLRATE) {
            init(pin, min_us, max_us, IOTEMPOWER_SERVO_DEFAULT_DURATION);
        }
        Emp_Servo(const char* name, uint8_t pin, int min_us, int max_us, int duration)
            : Device(name, IOTEMPIRE_SERVO_POLLRATE) {
            init(pin, min_us, max_us, duration);
        }
        Emp_Servo(const char* name, uint8_t pin) : Device (name, IOTEMPIRE_SERVO_POLLRATE) {
            init(pin, 554, 2400, IOTEMPOWER_SERVO_DEFAULT_DURATION);
        }
        Emp_Servo(const char* name, uint8_t pin, int duration) : Device (name, IOTEMPIRE_SERVO_POLLRATE) {
            init(pin, 554, 2400, duration);
        }
        void stop();
        Emp_Servo& turn_to(int value);
        Emp_Servo& set(int value);
        virtual void process(const Ustring& value);
        virtual bool measure(void);
        void start();
};


#endif // _EMP_SERVO_H_