// _Servo.h, ESP32 wrapper

#ifndef _Servo_h
#define _Servo_h

#include <Arduino.h>
#include <ESP32Servo.h>

class  _Servo
{
public:
     _Servo();
    ~ _Servo();
    uint8_t attach(int pin);
    uint8_t attach(int pin, uint16_t min, uint16_t max); // as above but also sets min and max values for writes. 
    void detach();
    void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
    void writeMicroseconds(int value); // Write pulse width in microseconds 
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds();            // returns current pulse width in microseconds for this servo (was read_us() in first release)
    bool attached();                   // return true if this servo is attached, otherwise false 
private:
    Servo esp32_servo;
    uint8_t  _pin;
};

#endif
