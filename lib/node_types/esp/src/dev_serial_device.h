// serial-device.h
// Base class for serial connector devices

#ifndef _IOTEMPOWER_SERIAL_DEVICE_H_
#define _IOTEMPOWER_SERIAL_DEVICE_H_

#include <device.h>
#include <SoftwareSerial.h>

#define IOTEMPOWER_SERIAL_DEVICE_DEFAULT_BAUD 9600

class Serial_Device : public Device {
    private:
        SoftwareSerial *serial = NULL;
        int8_t _rx_pin;
        int8_t _tx_pin;
        uint32_t _baud;
        SoftwareSerialConfig _config;
        bool _invert;
    protected:
        SoftwareSerial* serial_handle() { return serial; }
        void configure();
    public:
        Serial_Device(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = IOTEMPOWER_SERIAL_DEVICE_DEFAULT_BAUD,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false);
        ~Serial_Device();
        void start();
};

#endif // _IOTEMPOWER_SERIAL_DEVICE_H_
