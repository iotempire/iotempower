// IRReceiver.h
// Header File for controlling the IR receiver

#ifndef _IOTEMPOWER_IRReceiver_H_
#define _IOTEMPOWER_IRReceiver_H_

#include <Arduino.h>
#include <Wire.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <device.h>

class IRReceiver : public Device {
    private:
        IRrecv *sensor = NULL;
        long _signalPort;
    public:
        IRReceiver(const char* name, long signalPort);
        void start();
        bool measure();
};

#endif // _IOTEMPOWER_IRReceiver_H_