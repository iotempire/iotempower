// serial-socket.cpp
#include "dev_serial_socket.h"

Serial_Socket::Serial_Socket(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert) :
    Device(name, 100000) {
    _rx_pin = rx_pin;
    _tx_pin = tx_pin;
    _baud = baud;
    _config = config;
    _invert = invert;

    add_subdevice(new Subdevice()); // 0
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb(
        [&] (const Ustring& payload) {
            if(serial) {
                serial->write((const uint8_t*)payload.as_cstr(), payload.length());
                return true;
            }
            return false;
        }
    );
}

void Serial_Socket::start() {
    serial = new SoftwareSerial(_rx_pin, _tx_pin, _invert);
    if(serial) {
        serial->begin(_baud, _config, _rx_pin, _tx_pin, _invert);
        if(serial->operator bool()) {
            _started = true;
        } else {
            ulog(F("Invalid SoftwareSerial pin configuration."));
        }
    } else {
        ulog(F("Can't reserve memory for the serial socket."));
    }
}

bool Serial_Socket::measure() {
    if(!serial) return false;
    int available = serial->available();
    if(available <= 0) return false;

    Ustring& v = value();
    v.clear();
    bool has_data = false;
    while(serial->available() > 0) {
        int next = serial->read();
        if(next < 0) break;
        has_data = true;
        if(v.length() < v.max_length()) {
            v.add((char)next);
        }
    }
    return has_data;
}
