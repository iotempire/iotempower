// pwm.h
// change pwm parameters on a gpio port


#ifndef _PWM_H_
#define _PWM_H_

#include <_PWM.h>
#include <device.h>

class Pwm : public Device {
    private:
        _PWM *_pwm;
        int _frequency = 1000;
        int _duty = 0;
    public:
        Pwm(const char* name, uint8_t pin, int frequency=1000);
        Pwm& set_duty(int duty) {
            set(duty,_frequency);
            return *this;
        }
        Pwm& with_duty(int duty) {
            set_duty(duty);
            return *this;
        }
        Pwm& set_frequency(int frequency) {
            // analogWriteFreq(_pin, frequency);
            set(_duty, frequency);
            return *this;
        }
        Pwm& with_frequency(int frequency) {
            set_frequency(frequency);
            return *this;
        }
        Pwm& set(int duty, int frequency);
        void start() {
            _started = true;
            set(_duty, _frequency);
        }
};

// TODO: add device for creating tones (just control by frequency and use duration)


#endif // _PWM_H_