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
    const uint32_t baud = baud_rate();
    const uint32_t idle_ms = max<uint32_t>(2, (100000UL / max<uint32_t>(baud, 1))); // ~10 bytes at 8N1

    // We build one serial frame across multiple measure() calls.
    // Frame boundary is detected by line idle time.
    // Non-blocking frame collection: read only what is currently available.
    bool got_new_byte = false;
    while(serial->available() > 0) {
        int next = serial->read();
        if(next < 0) break;
        got_new_byte = true;
        _frame_active = true;
        _last_rx_ms = millis();
        if(!_frame_overflow) {
            if(!_rx_frame.add_hex((uint8_t)next)) {
                // Keep collecting until frame end, but never publish a cut frame.
                _frame_overflow = true;
            }
        }
    }

    // Keep waiting until line idle marks end-of-frame.
    if(got_new_byte) return false;
    if(!_frame_active) return false;
    if((uint32_t)(millis() - _last_rx_ms) < idle_ms) return false;

    _frame_active = false;
    if(_frame_overflow) {
        _frame_overflow = false;
        _rx_frame.clear();
        ulog(F("Serial_Hex frame dropped: overflow."));
        return false;
    }

    Ustring& v = value();
    v.copy(_rx_frame);
    _rx_frame.clear();
    return !v.empty();
}
