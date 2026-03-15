// gmp343.cpp
#include "dev_gmp343.h"

static const char* GMP343_FORM_CMD = "FORM CO2 \" \" \"ppm\" #r#n";
static const char GMP343_PROMPT = '>';

GMP343::GMP343(const char* name, int8_t rx_pin, int8_t tx_pin,
        uint32_t baud, SoftwareSerialConfig config, bool invert,
        uint16_t interval_ms, uint16_t timeout_ms, uint8_t retries) :
    Serial_Device(name, rx_pin, tx_pin, baud, config, invert) {
    _interval_ms = interval_ms;
    _timeout_ms = timeout_ms;
    _retries = retries;
    add_subdevice(new Subdevice());
    pollrate(100); // fast enough to progress serial state machine without blocking
}

void GMP343::clear_input() {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return;
    while(serial->available() > 0) {
        serial->read();
    }
}

void GMP343::reset_reply() {
    _reply.clear();
}

bool GMP343::send_command(const char* cmd) {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;

    clear_input();
    reset_reply();

    serial->write((const uint8_t*)cmd, strlen(cmd));
    serial->write('\r');
    _command_started_ms = millis();
    return true;
}

bool GMP343::collect_until_prompt() {
    SoftwareSerial* serial = serial_handle();
    if(!serial) return false;

    while(serial->available() > 0) {
        int next = serial->read();
        if(next < 0) break;
        if(next == GMP343_PROMPT) {
            return true;
        }
        if(!_reply.add((char)next)) {
            // Reply is small in normal operation. If it overflows, restart command.
            return false;
        }
    }
    return false;
}

bool GMP343::command_timed_out() const {
    return (uint32_t)(millis() - _command_started_ms) >= _timeout_ms;
}

static bool gmp343_is_num_char(char c) {
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.';
}

bool GMP343::parse_co2_reply(float& ppm) {
    const char* data = _reply.as_cstr();
    int len = _reply.length();

    int ppm_pos = -1;
    for(int i = 0; i + 2 < len; i++) {
        char c0 = data[i];
        char c1 = data[i + 1];
        char c2 = data[i + 2];
        if((c0 == 'p' || c0 == 'P') &&
            (c1 == 'p' || c1 == 'P') &&
            (c2 == 'm' || c2 == 'M')) {
            ppm_pos = i;
            break;
        }
    }

    int scan_end = (ppm_pos >= 0) ? ppm_pos - 1 : len - 1;
    int number_end = -1;
    for(int i = scan_end; i >= 0; i--) {
        if(gmp343_is_num_char(data[i])) {
            number_end = i;
            break;
        }
    }
    if(number_end < 0) return false;

    int number_start = number_end;
    while(number_start > 0 && gmp343_is_num_char(data[number_start - 1])) {
        number_start--;
    }

    char number_buf[24];
    int out = 0;
    for(int i = number_start; i <= number_end && out < (int)sizeof(number_buf) - 1; i++) {
        number_buf[out++] = data[i];
    }
    number_buf[out] = 0;
    ppm = atof(number_buf);
    return out > 0;
}

void GMP343::schedule_retry(State retry_state) {
    _retry_index++;
    _state = retry_state;
    _next_send_ms = millis() + 200;
    reset_reply();
}

bool GMP343::measure() {
    if(!serial_handle()) return false;

    // Initialization sequence from working host script:
    // S -> SMODE STOP -> ECHO OFF -> FORM ...
    switch(_state) {
    case INIT_SEND_STOP:
        if(send_command("S")) {
            _state = INIT_WAIT_STOP;
        }
        return false;

    case INIT_WAIT_STOP:
        if(collect_until_prompt() || command_timed_out()) {
            _retry_index = 0;
            _state = INIT_SEND_MODE;
            reset_reply();
        }
        return false;

    case INIT_SEND_MODE:
        if(send_command("SMODE STOP")) {
            _state = INIT_WAIT_MODE;
        }
        return false;

    case INIT_WAIT_MODE:
        if(collect_until_prompt()) {
            _retry_index = 0;
            _state = INIT_SEND_ECHO_OFF;
            reset_reply();
        } else if(command_timed_out()) {
            if(_retry_index + 1 < ((_retries == 0) ? 1 : _retries)) {
                schedule_retry(INIT_SEND_MODE);
            } else {
                _retry_index = 0;
                _state = INIT_SEND_ECHO_OFF;
            }
        }
        return false;

    case INIT_SEND_ECHO_OFF:
        if(send_command("ECHO OFF")) {
            _state = INIT_WAIT_ECHO_OFF;
        }
        return false;

    case INIT_WAIT_ECHO_OFF:
        if(collect_until_prompt()) {
            _retry_index = 0;
            _state = INIT_SEND_FORM;
            reset_reply();
        } else if(command_timed_out()) {
            if(_retry_index + 1 < ((_retries == 0) ? 1 : _retries)) {
                schedule_retry(INIT_SEND_ECHO_OFF);
            } else {
                _retry_index = 0;
                _state = INIT_SEND_FORM;
            }
        }
        return false;

    case INIT_SEND_FORM:
        if(send_command(GMP343_FORM_CMD)) {
            _state = INIT_WAIT_FORM;
        }
        return false;

    case INIT_WAIT_FORM:
        if(collect_until_prompt()) {
            _retry_index = 0;
            _state = READY_IDLE;
            _next_send_ms = millis();
            reset_reply();
        } else if(command_timed_out()) {
            if(_retry_index + 1 < ((_retries == 0) ? 1 : _retries)) {
                schedule_retry(INIT_SEND_FORM);
            } else {
                _retry_index = 0;
                _state = READY_IDLE;
                _next_send_ms = millis();
            }
        }
        return false;

    case READY_IDLE:
        if((int32_t)(millis() - _next_send_ms) < 0) {
            return false;
        }
        if(send_command("SEND")) {
            _state = READY_WAIT_REPLY;
        }
        return false;

    case READY_WAIT_REPLY:
        if(collect_until_prompt()) {
            float ppm = 0.0f;
            if(parse_co2_reply(ppm)) {
                value().printf("%.1f", ppm);
                _state = READY_IDLE;
                _next_send_ms = millis() + _interval_ms;
                _retry_index = 0;
                reset_reply();
                return true;
            }

            if(_retry_index + 1 < ((_retries == 0) ? 1 : _retries)) {
                schedule_retry(READY_IDLE);
            } else {
                _state = READY_IDLE;
                _next_send_ms = millis() + _interval_ms;
                _retry_index = 0;
                reset_reply();
            }
            return false;
        }

        if(command_timed_out()) {
            if(_retry_index + 1 < ((_retries == 0) ? 1 : _retries)) {
                schedule_retry(READY_IDLE);
            } else {
                _state = READY_IDLE;
                _next_send_ms = millis() + _interval_ms;
                _retry_index = 0;
                reset_reply();
            }
        }
        return false;
    }

    return false;
}
