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
    const uint32_t baud = baud_rate();
    const uint32_t idle_ms = max<uint32_t>(2, (100000UL / max<uint32_t>(baud, 1))); // ~10 bytes at 8N1
    const uint32_t max_ms = min<uint32_t>(100, idle_ms * 3);
    uint32_t start_ms = millis();
    uint32_t last_rx_ms = start_ms;

    while(true) {
        while(serial->available() > 0) {
            int next = serial->read();
            if(next < 0) break;
            has_data = true;
            last_rx_ms = millis();
            if(v.length() < v.max_length()) {
                v.add((char)next);
            }
        }
        if(!has_data) break;
        uint32_t now = millis();
        if((uint32_t)(now - last_rx_ms) >= idle_ms) break;
        if((uint32_t)(now - start_ms) >= max_ms) break;
        delay(1);
    }
    return has_data;
}
