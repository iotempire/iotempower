// hx711.h

#ifndef _IOTEMPOWER_HX711_H_
#define _IOTEMPOWER_HX711_H_

#include <Arduino.h>
#include <device.h>
#include <HX711_ADC.h>


class Hx711 : public Device {
    private:
        int _precision = 1;
        uint8_t _sck_pin;
        uint8_t _dout_pin;
        float _calfactor;
        HX711_ADC *sensor;
        bool _calibration;
        bool first_tare_done = false;
        unsigned long last_read;
    public:
        Hx711(const char* name, uint8_t sck_pin, uint8_t doutpin,
            float calfactor=450.0, bool calibration=false);
        Hx711& with_precision(int precision) {
            if(precision < 1) precision = 1;
            _precision = precision;
            return *this;
        }
        void start();
        bool measure();
};


#endif // _IOTEMPOWER_HX711_H_