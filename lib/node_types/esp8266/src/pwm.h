// pwm.h
// change pwm parameters on a gpio port


#ifndef _PWM_H_
#define _PWM_H_

#include <device.h>

class Pwm : public Device {
    private:
        uint8_t _pin;
        int _frequency = 1000;
        int _duty = 0;
    public:
        Pwm(const char* name, uint8_t pin, int frequency=1000);
        void set_duty(int duty) {
            set(duty,_frequency);
        }
        Pwm& with_duty(int duty) {
            set_duty(duty);
            return *this;
        }
        void set_frequency(int frequency) {
            // analogWriteFreq(_pin, frequency);
            set(_duty, frequency);
        }
        Pwm& with_frequency(int frequency) {
            set_frequency(frequency);
            return *this;
        }
        void set(int duty, int frequency);
        void start() {
            _started = true;
            set(_duty, _frequency);
        }
};

// TODO: add device for creating tones (just control by frequency and use duration)


#endif // _PWM_H_