// pwm.h
// change pwm parameters on a gpio port


#ifndef _PWM_H_
#define _PWM_H_

// TODO: take another look at https://github.com/StefanBruens/ESP8266_new_pwm
// and eventually adapt it -> is GPL, so not usable
#include <rgb_base.h>
#include "core_esp8266_waveform.h"

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


#endif // _RGB_BASE_H_