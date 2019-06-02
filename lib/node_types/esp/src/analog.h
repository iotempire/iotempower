// analog.h
// Header File for analog device (analog port)

#ifndef _ANALOG_H_
#define _ANALOG_H_

#include <Arduino.h>
#include <device.h>

class Analog : public Device {
    private:
        const char* _high;
        const char* _low;
        int _pin;
        int _threshold = 0;
        int _precision = 1;
    public:
        Analog(const char* name) :
            Device(name) {
            add_subdevice(new Subdevice(""));
        }
        void start() {
            _started = true;
            measure();
        }
        bool measure();
        Analog& with_threshold(int threshold, const char* high, const char* low) {
            _high = high;
            _low = low;
            if(threshold > 1024) threshold=1024;
            else if(threshold < 0) threshold=0;
            _threshold = threshold;
            return *this;
        }
        Analog& with_threshold(int threshold) {
            return with_threshold(threshold, "on", "off");
        }
        Analog& with_precision(int precision) {
            if(precision < 1) precision = 1;
            _precision = precision;
            return *this;
        }
        // access measured_value()
        double read_float() { return measured_value().as_float(); }
        long read_int() { return measured_value().as_int(); }
        void write_float(double v) { measured_value().from(v); }
        void write_int(long v) { measured_value().from(v); }
};



#endif // _ANALOG_H_