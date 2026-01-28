// serial-socket.h
// Header File for creating an IoTempower serial connector socket
// (for example to connect to another MCU via software serial)

#ifndef _IOTEMPOWER_SERIAL_SOCKET_H_
#define _IOTEMPOWER_SERIAL_SOCKET_H_

#include <device.h>
#include <SoftwareSerial.h>

#define IOTEMPOWER_SERIAL_SOCKET_DEFAULT_BAUD 9600

class Serial_Socket : public Device {
    private:
        SoftwareSerial *serial = NULL;
        int8_t _rx_pin;
        int8_t _tx_pin;
        uint32_t _baud;
        SoftwareSerialConfig _config;
        bool _invert;
    public:
        Serial_Socket(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = IOTEMPOWER_SERIAL_SOCKET_DEFAULT_BAUD,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false);
        ~Serial_Socket();
        void start();
        bool measure();
};

#endif // _IOTEMPOWER_SERIAL_SOCKET_H_
