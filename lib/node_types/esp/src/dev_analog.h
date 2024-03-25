// analog.h
// Header File for analog device (analog port)

#ifndef _ANALOG_H_
#define _ANALOG_H_

#include <Arduino.h>
#include <device.h>

#define IOTEMPOWER_MAX_ANALOG_MUX 6

class Analog : public Device {
    private:
        int _pin;
        int _mux_pins[IOTEMPOWER_MAX_ANALOG_MUX];
        int _num_mux_pins=-1;
        Analog& _mux(int num_pins, ...);
    public:
        Analog(const char* name, int pin=A0):
            Device(name, 10000) {
            _pin=pin;
            add_subdevice(new Subdevice());
        }
        void start();
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
        Analog& mux(int mux1, int mux2=-1, int mux3=-1, int mux4=-1, int mux5=-1, int mux6=-1) {
            _num_mux_pins=1;
            if(mux6>=0) _mux(6, mux1, mux2, mux3, mux4, mux5, mux6);
            else if(mux5>=0) _mux(5, mux1, mux2, mux3, mux4, mux5);
            else if(mux4>=0) _mux(4, mux1, mux2, mux3, mux4);
            else if(mux3>=0) _mux(3, mux1, mux2, mux3);
            else if(mux2>=0) _mux(2, mux1, mux2);
            else if(mux1>=0) _mux(1, mux1);
            return *this;
        }
};



#endif // _ANALOG_H_