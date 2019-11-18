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
        MFRC522::MIFARE_Key key;
        uint16_t _data_size;
        uint16_t block_count;
        bool _in_hex;
        bool _use_auth;
        int presence_detect_tries = 0;
        Ustring next_write;
        bool write_requested = false; // TODO: add request time and expiration
    public:
        Mfrc522(const char* name, uint16_t data_size=0, 
            bool in_hex=false, bool use_auth=false, const byte* six_byte_key = NULL);
        void write(const Ustring &payload);
        void start();
        bool measure();
};


#endif // _IOTEMPOWER_MFRC522_H_