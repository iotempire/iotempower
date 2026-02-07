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
    while(serial->available()) {
        serial->read();
    }
}

bool Soil_7in1::read_exactly(uint8_t* buf, size_t n, uint16_t timeout_ms) {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;
    size_t got = 0;
    unsigned long start = millis();
    while(got < n && (uint16_t)(millis() - start) < timeout_ms) {
        if(serial->available()) {
            buf[got++] = (uint8_t)serial->read();
        }
        yield();
    }
    return got == n;
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

bool Soil_7in1::read_input_regs(uint16_t start_reg, uint16_t count, uint16_t* out) {
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

    serial->flush();
    clear_input();

    rs485_set_tx(true);
    serial->write(req, 8);
    serial->flush();
    rs485_set_tx(false);

    const uint8_t resp_len = (uint8_t)(5 + 2 * count);
    uint8_t resp[32];
    if(!read_exactly(resp, resp_len, _timeout_ms)) {
        return false;
    }

    if(resp[0] != _addr) return false;
    if(resp[1] & 0x80) return false;
    if(resp[1] != MODBUS_FUNC_READ_INPUT) return false;
    if(resp[2] != (uint8_t)(2 * count)) return false;

    uint16_t crc_resp = (uint16_t)resp[resp_len - 2] |
        ((uint16_t)resp[resp_len - 1] << 8);
    uint16_t crc_calc = modbus_crc16(resp, resp_len - 2);
    if(crc_resp != crc_calc) return false;

    for(uint16_t i = 0; i < count; i++) {
        uint8_t hi = resp[3 + 2 * i];
        uint8_t lo = resp[4 + 2 * i];
        out[i] = ((uint16_t)hi << 8) | (uint16_t)lo;
    }
    return true;
}

bool Soil_7in1::read_input_regs_retry(uint16_t start_reg, uint16_t count, uint16_t* out) {
    for(uint8_t i = 0; i < _retries; i++) {
        if(read_input_regs(start_reg, count, out)) return true;
        delay(50);
    }
    return false;
}

void Soil_7in1::set_value_if_enabled(int8_t sd, uint16_t raw, float divisor, uint8_t decimals) {
    if(sd < 0) return;
    if(divisor <= 0.0f) divisor = 1.0f;
    float v = raw / divisor;
    value(sd).printf("%.*f", decimals, v);
}

bool Soil_7in1::measure() {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;
    if(!_moisture && !_temperature && !_ec && !_ph &&
        !_nitrogen && !_phosphorus && !_potassium && !_salinity && !_tds) {
        return false;
    }

    uint16_t regs[9] = {0};
    if(!read_input_regs_retry(0x0000, 9, regs)) {
        return false;
    }

    bool has_data = false;
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
