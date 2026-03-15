// gmp343.h
// Vaisala GMP343 CO2 sensor over serial

#ifndef _IOTEMPOWER_GMP343_H_
#define _IOTEMPOWER_GMP343_H_

#include <Arduino.h>
#include "dev_serial_device.h"

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
        bool send_command(const char* cmd);
        bool collect_until_prompt();
        bool command_timed_out() const;
        bool parse_co2_reply(float& ppm);
        void schedule_retry(State retry_state);
        void reset_reply();

    public:
        GMP343(const char* name, int8_t rx_pin, int8_t tx_pin,
            uint32_t baud = 19200,
            SoftwareSerialConfig config = SWSERIAL_8N1,
            bool invert = false,
            uint16_t interval_ms = 1000,
            uint16_t timeout_ms = 1500,
            uint8_t retries = 3);

        bool measure();
};

#endif // _IOTEMPOWER_GMP343_H_
