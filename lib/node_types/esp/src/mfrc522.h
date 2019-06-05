// mfrc522.h

#ifndef _IOTEMPOWER_MFRC522_H_
#define _IOTEMPOWER_MFRC522_H_

#include <Arduino.h>
#include <device.h>
#include <SPI.h>
#include <MFRC522.h>


class Mfrc522 : public Device {
    private:
        MFRC522 *sensor;
        unsigned long last_read;
    public:
        Mfrc522(const char* name);
        void start();
        bool measure();
};


#endif // _IOTEMPOWER_MFRC522_H_