// emp_hx711.h

#ifndef _IOTEMPOWER_HX711_H_
#define _IOTEMPOWER_HX711_H_

#include <Arduino.h>
#include <device.h>
#include <HX711.h>
#include <filter.h> // TODO: check why this needs to be included explicitely


class Hx711 : public Device {
    private:
        float _precision = 0.1;
        uint8_t _sck_pin;
        uint8_t _dout_pin;
        float _calfactor;
        HX711 sensor;
        bool _calibration;
        bool first_tare_done = false; // TODO: better tare
        unsigned long last_read;
        float lastweight = -10000;
    public:
        Hx711(const char* name, uint8_t sck_pin, uint8_t doutpin,
            float calfactor=450.0, bool calibration=false);
        Hx711& with_precision(float precision) {
            if(precision < 1) precision = 1;
            return (Hx711&)with_filter_callback(*new Filter_Minchange<float>(precision));
        }
        Hx711& precision(float precision) {
            return with_precision(precision);
        }
        void tare();
        void start();
        bool measure();
};


#endif // _IOTEMPOWER_HX711_H_
