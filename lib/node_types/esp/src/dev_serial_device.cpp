// serial-device.cpp
#include "dev_serial_device.h"

Serial_Device::Serial_Device(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert) :
    Device(name, 100000) {
    _rx_pin = rx_pin;
    _tx_pin = tx_pin;
    _baud = baud;
    _config = config;
    _invert = invert;
}

void Serial_Device::configure() {
    if(serial) {
        serial->begin(_baud, _config);
    }
}

Serial_Device::~Serial_Device() {
    if(serial) {
        delete serial;
        serial = NULL;
    }
}

void Serial_Device::start() {
    serial = new SoftwareSerial(_rx_pin, _tx_pin, _invert);
    if(serial) {
        configure();
        if(serial->operator bool()) {
            _started = true;
        } else {
            ulog(F("Invalid SoftwareSerial pin configuration."));
        }
    } else {
        ulog(F("Can't reserve memory for the serial device."));
    }
}
