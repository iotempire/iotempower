// serial-text.h
// Serial device that publishes text data

#ifndef _IOTEMPOWER_SERIAL_TEXT_H_
#define _IOTEMPOWER_SERIAL_TEXT_H_

#include "dev_serial_device.h"

class Serial_Text : public Serial_Device {
    public:
        Serial_Text(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = IOTEMPOWER_SERIAL_DEVICE_DEFAULT_BAUD,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false);
        bool measure();
};

#endif // _IOTEMPOWER_SERIAL_TEXT_H_
