// dsm501a.h
// DSM501A dust sensor using low-pulse occupancy on two output pins

#ifndef _IOTEMPOWER_DSM501A_H_
#define _IOTEMPOWER_DSM501A_H_

#include <Arduino.h>
#include <device.h>

#define DSM501A_DEFAULT_SAMPLETIME_MS 30000

class DSM501A : public Device {
    private:
        uint8_t _pm1_pin;
        uint8_t _pm25_pin;
        uint32_t _sampletime_ms = DSM501A_DEFAULT_SAMPLETIME_MS;
        uint32_t _sample_started_ms = 0;

        volatile uint32_t _pm1_lowpulse_us = 0;
        volatile uint32_t _pm25_lowpulse_us = 0;
        volatile uint32_t _pm1_low_started_us = 0;
        volatile uint32_t _pm25_low_started_us = 0;
        volatile bool _pm1_low_active = false;
        volatile bool _pm25_low_active = false;

        void pm1_changed();
        void pm25_changed();
        float calculate_concentration(uint32_t lowpulse_us) const;

    public:
        DSM501A(const char* name, uint8_t pm1_pin, uint8_t pm25_pin);
        void start() override;
        bool measure() override;
};

#endif // _IOTEMPOWER_DSM501A_H_
