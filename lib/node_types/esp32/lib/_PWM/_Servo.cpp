// _Servo.cpp
#include <Arduino.h>
#include <_Servo.h>


_Servo:: _Servo()
{
}

_Servo::~ _Servo() {
  detach();
}


uint8_t  _Servo::attach(int pin)
{
  if (esp32_servo.attached()) {
      detach();
  }
  _pin = pin;
  return esp32_servo.attach(pin);
}

uint8_t  _Servo::attach(int pin, uint16_t minUs, uint16_t maxUs)
{
  if (esp32_servo.attached()) {
      detach();
  }
  _pin = pin;
  return esp32_servo.attach(pin);
}

void  _Servo::detach()
{
    esp32_servo.detach();
}

void  _Servo::write(int value)
{
    esp32_servo.write(value);
}

void  _Servo::writeMicroseconds(int value)
{
    esp32_servo.writeMicroseconds(value);
}

int  _Servo::read() // return the value as degrees
{
    return esp32_servo.read();
}

int  _Servo::readMicroseconds()
{
  return esp32_servo.readMicroseconds();
}

bool  _Servo::attached()
{
  return esp32_servo.attached();
}
