// analog.h
// Header File for analog device (analog port)

#ifndef _ANALOG_H_
#define _ANALOG_H_

#include <Arduino.h>
#include <device.h>

class Analog : public Device {
    private:
        int _pin;
    public:
        Analog(const char* name) :
            Device(name, 10000) {
            add_subdevice(new Subdevice(""));
        }
        void start() {
            _started = true;
            measure();
        }
        bool measure();
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
};



#endif // _ANALOG_H_