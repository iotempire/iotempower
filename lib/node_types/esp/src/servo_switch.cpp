// servo_switch.cpp

#include "servo_switch.h"

void Servo_Switch::init(int on_angle, int off_angle, int return_angle,
        const char* on_command, const char* off_command) {
    _on_angle = on_angle;
    _off_angle = off_angle;
    _return_angle = return_angle;
    _on_command = on_command;
    _off_command = off_command;
}

Servo_Switch& Servo_Switch::set(const Ustring& status) {
    int to_angle = -1;

    if(status.equals(_on_command)) {
        to_angle = _on_angle;
    } else if (status.equals(_off_command)) {
        to_angle = _off_angle;
    }
    if(to_angle >= 0) {
        if(_return_angle >= 0) {
            needs_return = true;
        }
        turn_to(to_angle);
        value(0).copy(status);
    }
    return *this;
}

void Servo_Switch::process(const Ustring& value) {
    set(value);
}

bool Servo_Switch::measure() {
    // if there is duration left and the duration left is smaller than the 
    // time that has passed since start of the turn
    if(_duration > 0 && (millis() - start_time > (unsigned long)_duration)) {
        if(needs_return) {
            needs_return = false;
            turn_to(_return_angle);
        } else stop();
    }
    return false;
}
