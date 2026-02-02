// serial-hex.cpp
#include "dev_serial_hex.h"

static int serial_hex_nibble(char c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

Serial_Hex::Serial_Hex(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert) :
    Serial_Device(name, rx_pin, tx_pin, baud, config, invert) {
    add_subdevice(new Subdevice()); // 0
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb(
        [&] (const Ustring& payload) {
            SoftwareSerial* serial = serial_handle();
            if(!serial) return false;
            const char* data = payload.as_cstr();
            int len = payload.length();
            int i = 0;
            bool wrote = false;
            while(i < len) {
                while(i < len && data[i] <= ' ') {
                    i++;
                }
                if(i >= len) break;
                int hi = serial_hex_nibble(data[i]);
                if(hi < 0) break;
                i++;
                while(i < len && data[i] <= ' ') {
                    i++;
                }
                if(i >= len) break;
                int lo = serial_hex_nibble(data[i]);
                if(lo < 0) break;
                i++;
                serial->write((uint8_t)((hi << 4) | lo));
                wrote = true;
            }
            return wrote;
        }
    );
}

bool Serial_Hex::measure() {
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
            if(v.length() < v.max_length() - 1) {
                v.add_hex((uint8_t)next);
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
