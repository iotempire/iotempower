// dev_input_analog.h
// Header File for analog device (analog port)

#ifndef _ANALOG_H_
#define _ANALOG_H_

#include <Arduino.h>
#include <dev_input_base.h>

class Analog : public Input_Base {
    public:
        Analog(const char* name, int pin=A0):
            Input_Base(name, pin, 10000) {}
        void start();
//        bool measure();
        Analog& with_threshold(int threshold, const char* high=str_on, const char* low=str_off) {
            if(threshold > 1024) threshold=1024;
            else if(threshold < 0) threshold=0;
            return (Analog&)filter_binarize(threshold,high,low);
        }
        Analog& threshold(int threshold, const char* high=str_on, const char* low=str_off) {
            return with_threshold(threshold, high, low);
        }
        Analog& with_precision(int precision) {
            if(precision < 1) precision = 1;
            return (Analog&)with_filter_callback(*new Filter_Minchange<int>(precision));
        }
        Analog& precision(int precision) {
            return with_precision(precision);
        }

        int read() { return fill_buffer(analogRead(_pin)); }
};



#endif // _ANALOG_H_