// dev_input_base.h
// Header File for base of input device (simple gpio, button, contact, or analog input)

#ifndef _INPUT_BASE_H_
#define _INPUT_BASE_H_

#include <Arduino.h>
#include <device.h>

#define IOTEMPOWER_MAX_MUX 6

class Input_Base : public Device {
    private:
        int _mux_pins[IOTEMPOWER_MAX_MUX];
        int _num_mux_pins=-1;
        int _current_mux = 0;
        Input_Base& _mux(int num_pins, ...);
    protected:
        int _pin=-1;
        int _precise_reads = 0; // number of samples to store in buffer (also buffer size)
        int* buffer; // pointer to buffer
        int buffer_fill = 0; // number of samples currently stored in buffer
    public:
        Input_Base(const char* name, int pin, unsigned long pollrate_us = 10000) :
            Device(name, pollrate_us) { // faster default pollrate (1ms) than other devices
            _pin = pin;
            add_subdevice(new Subdevice());
        }

        void buffer_reset();
        int* get_buffer() { return buffer; }
        int get_buffer_fill() {return buffer_fill; }
        int get_buffer_size() {return _precise_reads; }
        bool has_buffer() { return get_buffer_size() > 0; }
        bool measure();
        Input_Base& precise_buffer(unsigned long interval_ms, unsigned int reads);

        int fill_buffer(int val) {
            if (has_buffer())
            {
                // only add if still space
                if (buffer_fill < _precise_reads) {
                    buffer[buffer_fill] = val;
                    buffer_fill ++;
                }
            }
            return val;
        };

        Input_Base& mux(int mux1, int mux2=-1, int mux3=-1, int mux4=-1, int mux5=-1, int mux6=-1) {
            _num_mux_pins=1;
            if(mux6>=0) _mux(6, mux1, mux2, mux3, mux4, mux5, mux6);
            else if(mux5>=0) _mux(5, mux1, mux2, mux3, mux4, mux5);
            else if(mux4>=0) _mux(4, mux1, mux2, mux3, mux4);
            else if(mux3>=0) _mux(3, mux1, mux2, mux3);
            else if(mux2>=0) _mux(2, mux1, mux2);
            else if(mux1>=0) _mux(1, mux1);
            return *this;
        }

        virtual int read() { return -1; } // return -1 if not overwritten - TODO: it complains without default -check, why

        virtual void start();
};

#include "filter.h"

#endif // _INPUT_BASE_H_