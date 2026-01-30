// serial-text.cpp
#include "dev_serial_text.h"

Serial_Text::Serial_Text(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert) :
    Serial_Device(name, rx_pin, tx_pin, baud, config, invert) {
    add_subdevice(new Subdevice()); // 0
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb(
        [&] (const Ustring& payload) {
            SoftwareSerial* serial = serial_handle();
            if(serial) {
                serial->write((const uint8_t*)payload.as_cstr(), payload.length());
                return true;
            }
            return false;
        }
    );
}

bool Serial_Text::measure() {
    SoftwareSerial* serial = serial_handle();
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
