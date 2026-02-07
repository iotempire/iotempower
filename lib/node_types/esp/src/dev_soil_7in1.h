// soil-7in1.h
// 7-in-1 soil sensor over RS485 Modbus (input registers)

#ifndef _IOTEMPOWER_SOIL_7IN1_H_
#define _IOTEMPOWER_SOIL_7IN1_H_

#include <Arduino.h>
#include "dev_serial_device.h"

class Soil_7in1 : public Serial_Device {
    private:
        uint8_t _addr = 0x01;
        int8_t _dir_pin = -1;
        uint16_t _timeout_ms = 400;
        uint8_t _retries = 3;
        bool _moisture = true;
        bool _temperature = true;
        bool _ec = true;
        bool _ph = true;
        bool _nitrogen = true;
        bool _phosphorus = true;
        bool _potassium = true;
        bool _salinity = true;
        bool _tds = true;

        int8_t _sd_moisture = -1;
        int8_t _sd_temperature = -1;
        int8_t _sd_ec = -1;
        int8_t _sd_ph = -1;
        int8_t _sd_n = -1;
        int8_t _sd_p = -1;
        int8_t _sd_k = -1;
        int8_t _sd_salinity = -1;
        int8_t _sd_tds = -1;

        void rs485_set_tx(bool tx);
        void clear_input();
        bool read_exactly(uint8_t* buf, size_t n, uint16_t timeout_ms);
        uint16_t modbus_crc16(const uint8_t* data, size_t len);
        bool read_input_regs(uint16_t start_reg, uint16_t count, uint16_t* out);
        bool read_input_regs_retry(uint16_t start_reg, uint16_t count, uint16_t* out);

        void set_value_if_enabled(int8_t sd, uint16_t raw, float divisor, uint8_t decimals);
    public:
        Soil_7in1(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = 4800,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false,
            uint8_t addr = 0x01,
            int8_t dir_pin = -1,
            uint16_t timeout_ms = 400,
            uint8_t retries = 3,
            bool moisture = true,
            bool temperature = true,
            bool ec = true,
            bool ph = true,
            bool nitrogen = true,
            bool phosphorus = true,
            bool potassium = true,
            bool salinity = true,
            bool tds = true);

        void start() override;
        bool measure();
};

#endif // _IOTEMPOWER_SOIL_7IN1_H_
