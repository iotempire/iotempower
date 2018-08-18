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
            // if(duty>1020) duty=1023; // crashes when set to 1021 or 1022 TODO: why?!
            // analogWrite(_pin, _duty);
            set(duty,_frequency);
        }
        void set_frequency(int frequency) {
            // analogWriteFreq(_pin, frequency);
            set(_duty, frequency);
        }
        void set(int duty, int frequency);
};

// TODO: add device for creating tones (just control by frequency and use duration)


#endif // _PWM_H_