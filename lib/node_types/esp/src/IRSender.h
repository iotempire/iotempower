// IRSender.h
// Header File for controlling the IR sender/actor

#ifndef _IOTEMPOWER_IRSender_H_
#define _IOTEMPOWER_IRSender_H_

#include <Arduino.h>
#include <Wire.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <device.h>

class IRSender : public Device {
    private:
        IRsend *sensor = NULL;
        long _nec;
        long _signalPort;
    public:
        IRSender(const char* name, long nec, long signalPort);
        void start();
        bool measure();
};

#endif // _IOTEMPOWER_IRSender_H_