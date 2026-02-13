// serial-hex.h
// Serial device that publishes hex-encoded data

#ifndef _IOTEMPOWER_SERIAL_HEX_H_
#define _IOTEMPOWER_SERIAL_HEX_H_

#include "dev_serial_device.h"

class Serial_Hex : public Serial_Device {
    private:
        Ustring _rx_frame;
        bool _frame_active = false;
        bool _frame_overflow = false;
        uint32_t _last_rx_ms = 0;
    public:
        Serial_Hex(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = IOTEMPOWER_SERIAL_DEVICE_DEFAULT_BAUD,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false);
        bool measure();
};

#endif // _IOTEMPOWER_SERIAL_HEX_H_
