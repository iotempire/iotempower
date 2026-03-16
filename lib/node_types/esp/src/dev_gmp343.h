// gmp343.h
// Vaisala GMP343 CO2 sensor over serial

#ifndef _IOTEMPOWER_GMP343_H_
#define _IOTEMPOWER_GMP343_H_

#include <Arduino.h>
#include "dev_serial_device.h"

#define GMP343_DEFAULT_BAUD 19200
#define GMP343_DEFAULT_INTERVAL_MS 1000
#define GMP343_DEFAULT_TIMEOUT_MS 1500
#define GMP343_DEFAULT_RETRIES 3

class GMP343 : public Serial_Device {
    private:
        enum State {
            INIT_SEND_STOP,
            INIT_WAIT_STOP,
            INIT_SEND_MODE,
            INIT_WAIT_MODE,
            INIT_SEND_ECHO_OFF,
            INIT_WAIT_ECHO_OFF,
            INIT_SEND_FORM,
            INIT_WAIT_FORM,
            READY_IDLE,
            READY_WAIT_REPLY
        };

        State _state = INIT_SEND_STOP;
        Ustring _reply;
        uint32_t _command_started_ms = 0;
        uint32_t _next_send_ms = 0;
        uint16_t _timeout_ms = 1500;
        uint16_t _interval_ms = 1000;
        uint8_t _retries = 3;
        uint8_t _retry_index = 0;

        void clear_input();
        bool send_command(const __FlashStringHelper* cmd);
        bool collect_until_prompt();
        bool command_timed_out() const;
        bool parse_co2_reply(float& ppm);
        void schedule_retry(State retry_state);
        void reset_reply();

    public:
        GMP343(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = GMP343_DEFAULT_BAUD,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false);

        bool measure();
};

#endif // _IOTEMPOWER_GMP343_H_
