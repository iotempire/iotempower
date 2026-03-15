// soil-7in1.cpp
#include "dev_soil_7in1.h"

static const uint8_t MODBUS_FUNC_READ_INPUT = 0x04;

Soil_7in1::Soil_7in1(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert,
        uint8_t addr, int8_t dir_pin,
        uint16_t timeout_ms, uint8_t retries,
        bool moisture, bool temperature, bool ec, bool ph,
        bool nitrogen, bool phosphorus, bool potassium, bool salinity, bool tds) :
    Serial_Device(name, rx_pin, tx_pin, baud, config, invert) {
    _addr = addr;
    _dir_pin = dir_pin;
    _timeout_ms = timeout_ms;
    _retries = retries;
    _moisture = moisture;
    _temperature = temperature;
    _ec = ec;
    _ph = ph;
    _nitrogen = nitrogen;
    _phosphorus = phosphorus;
    _potassium = potassium;
    _salinity = salinity;
    _tds = tds;

    int8_t subd_nr = 0;
    if(_moisture) {
        add_subdevice(new Subdevice(F("moisture")));
        _sd_moisture = subd_nr++;
    }
    if(_temperature) {
        add_subdevice(new Subdevice(F("temperature")));
        _sd_temperature = subd_nr++;
    }
    if(_ec) {
        add_subdevice(new Subdevice(F("ec")));
        _sd_ec = subd_nr++;
    }
    if(_ph) {
        add_subdevice(new Subdevice(F("ph")));
        _sd_ph = subd_nr++;
    }
    if(_nitrogen) {
        add_subdevice(new Subdevice(F("nitrogen")));
        _sd_n = subd_nr++;
    }
    if(_phosphorus) {
        add_subdevice(new Subdevice(F("phosphorus")));
        _sd_p = subd_nr++;
    }
    if(_potassium) {
        add_subdevice(new Subdevice(F("potassium")));
        _sd_k = subd_nr++;
    }
    if(_salinity) {
        add_subdevice(new Subdevice(F("salinity")));
        _sd_salinity = subd_nr++;
    }
    if(_tds) {
        add_subdevice(new Subdevice(F("tds")));
        _sd_tds = subd_nr++;
    }

    pollrate(2000);
}

void Soil_7in1::start() {
    Serial_Device::start();
    if(_started && _dir_pin >= 0) {
        // For manual-direction RS485 transceivers (DE/RE pin).
        pinMode(_dir_pin, OUTPUT);
        rs485_set_tx(false);
    }
}

void Soil_7in1::rs485_set_tx(bool tx) {
    if(_dir_pin < 0) return;
    digitalWrite(_dir_pin, tx ? HIGH : LOW);
    delayMicroseconds(50);
}

void Soil_7in1::clear_input() {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return;
    // Drop stale bytes from previous/incomplete responses before new request.
    while(serial->available()) {
        serial->read();
    }
}

uint16_t Soil_7in1::modbus_crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for(size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++) {
            if(crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

bool Soil_7in1::start_read_input_regs(uint16_t start_reg, uint16_t count) {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;
    if(count == 0 || count > 9) return false;

    uint8_t req[8];
    req[0] = _addr;
    req[1] = MODBUS_FUNC_READ_INPUT;
    req[2] = (uint8_t)(start_reg >> 8);
    req[3] = (uint8_t)(start_reg & 0xFF);
    req[4] = (uint8_t)(count >> 8);
    req[5] = (uint8_t)(count & 0xFF);
    uint16_t crc = modbus_crc16(req, 6);
    req[6] = (uint8_t)(crc & 0xFF);
    req[7] = (uint8_t)(crc >> 8);

    // Ensure response buffer starts clean for this transaction.
    clear_input();

    // RS485 request frame:
    // [addr][0x04][regHi][regLo][countHi][countLo][crcLo][crcHi]
    rs485_set_tx(true);
    serial->write(req, 8);
    rs485_set_tx(false);

    _expected_len = (uint8_t)(5 + 2 * count);
    _rx_len = 0;
    _request_started_ms = millis();
    _state = WAIT_RESPONSE;
    // Transaction started. Response is collected over multiple measure() calls.
    return true;
}

bool Soil_7in1::collect_response() {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;
    // Non-blocking read: consume only currently available bytes.
    while(_rx_len < _expected_len && serial->available() > 0) {
        int next = serial->read();
        if(next < 0) break;
        _resp[_rx_len++] = (uint8_t)next;
    }
    return _rx_len >= _expected_len;
}

bool Soil_7in1::validate_and_unpack(uint16_t count, uint16_t* out) {
    // Validate full Modbus RTU frame before publishing any value.
    // Expected response:
    // [addr][0x04][byteCount=2*count][data...][crcLo][crcHi]
    if(_rx_len != _expected_len) return false;
    if(_resp[0] != _addr) return false;
    if(_resp[1] & 0x80) return false;
    if(_resp[1] != MODBUS_FUNC_READ_INPUT) return false;
    if(_resp[2] != (uint8_t)(2 * count)) return false;

    uint16_t crc_resp = (uint16_t)_resp[_expected_len - 2] |
        ((uint16_t)_resp[_expected_len - 1] << 8);
    uint16_t crc_calc = modbus_crc16(_resp, _expected_len - 2);
    if(crc_resp != crc_calc) return false;

    // Data is big-endian register stream.
    for(uint16_t i = 0; i < count; i++) {
        uint8_t hi = _resp[3 + 2 * i];
        uint8_t lo = _resp[4 + 2 * i];
        out[i] = ((uint16_t)hi << 8) | (uint16_t)lo;
    }
    return true;
}

void Soil_7in1::schedule_retry() {
    // Retry later without blocking the main loop.
    _retry_index++;
    _state = WAIT_RETRY;
    _retry_after_ms = millis() + 50;
}

void Soil_7in1::reset_read_state() {
    // Return to neutral state after success/failure.
    _state = IDLE;
    _retry_index = 0;
    _rx_len = 0;
    _expected_len = 0;
}

void Soil_7in1::set_value_if_enabled(int8_t sd, uint16_t raw, float divisor, uint8_t decimals) {
    if(sd < 0) return;
    if(divisor <= 0.0f) divisor = 1.0f;
    float v = raw / divisor;
    value(sd).printf("%.*f", decimals, v);
}

bool Soil_7in1::measure() {
    if(!serial_handle()) return false;
    // If all channels are disabled, skip polling entirely.
    if(!_moisture && !_temperature && !_ec && !_ph &&
        !_nitrogen && !_phosphorus && !_potassium && !_salinity && !_tds) {
        return false;
    }

    uint16_t regs[9] = {0};
    const uint16_t reg_count = 9;
    const uint8_t max_attempts = (_retries == 0) ? 1 : _retries;

    // State machine overview:
    // IDLE -> send request and switch to WAIT_RESPONSE.
    // WAIT_RESPONSE -> accumulate bytes until full frame or timeout.
    // WAIT_RETRY -> short cooldown before issuing retry request.

    // Start a new transaction.
    if(_state == IDLE) {
        if(!start_read_input_regs(0x0000, reg_count)) {
            reset_read_state();
        }
        return false;
    }

    // Inter-retry delay handled by timestamp.
    if(_state == WAIT_RETRY) {
        if((int32_t)(millis() - _retry_after_ms) < 0) {
            return false;
        }
        if(!start_read_input_regs(0x0000, reg_count)) {
            reset_read_state();
        }
        return false;
    }

    if(_state != WAIT_RESPONSE) return false;

    // Wait for full response or timeout, one loop step at a time.
    if(collect_response()) {
        if(validate_and_unpack(reg_count, regs)) {
            // Full valid frame received; publish path below can proceed.
            reset_read_state();
        } else if(_retry_index + 1 < max_attempts) {
            // Corrupt/invalid frame: retry later.
            schedule_retry();
            return false;
        } else {
            // Retry budget exhausted.
            reset_read_state();
            return false;
        }
    } else if((uint32_t)(millis() - _request_started_ms) >= _timeout_ms) {
        // Frame not complete within timeout.
        if(_retry_index + 1 < max_attempts) {
            schedule_retry();
            return false;
        }
        reset_read_state();
        return false;
    } else {
        // Still waiting for more bytes.
        return false;
    }

    bool has_data = false;
    // Register map:
    // 0 moisture, 1 temperature, 2 ec, 3 ph, 4 n, 5 p, 6 k, 7 salinity, 8 tds
    if(_sd_moisture >= 0) {
        set_value_if_enabled(_sd_moisture, regs[0], 10.0f, 1);
        has_data = true;
    }
    if(_sd_temperature >= 0) {
        set_value_if_enabled(_sd_temperature, regs[1], 10.0f, 1);
        has_data = true;
    }
    if(_sd_ec >= 0) {
        set_value_if_enabled(_sd_ec, regs[2], 10.0f, 1);
        has_data = true;
    }
    if(_sd_ph >= 0) {
        set_value_if_enabled(_sd_ph, regs[3], 10.0f, 1);
        has_data = true;
    }
    if(_sd_n >= 0) {
        set_value_if_enabled(_sd_n, regs[4], 1.0f, 0);
        has_data = true;
    }
    if(_sd_p >= 0) {
        set_value_if_enabled(_sd_p, regs[5], 1.0f, 0);
        has_data = true;
    }
    if(_sd_k >= 0) {
        set_value_if_enabled(_sd_k, regs[6], 1.0f, 0);
        has_data = true;
    }
    if(_sd_salinity >= 0) {
        set_value_if_enabled(_sd_salinity, regs[7], 1.0f, 0);
        has_data = true;
    }
    if(_sd_tds >= 0) {
        set_value_if_enabled(_sd_tds, regs[8], 1.0f, 0);
        has_data = true;
    }

    return has_data;
}
