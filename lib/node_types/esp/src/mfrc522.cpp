// mfrc522.cpp
#include "mfrc522.h"

void Mfrc522::write(const Ustring &payload) {

    byte block=0;
    uint16_t left = payload.length();
    byte buffer[18];
    byte buffer_fill=0;
    int pos = 0;

    while(left>0) {

        if(_use_auth) {
            // authenticate
            MFRC522::StatusCode status = sensor->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
                    block, &key, &(sensor->uid)); //line 834
            if (status != MFRC522::STATUS_OK) {
                ulog(F("MFRC522 Authentication failed: %s"),
                    sensor->GetStatusCodeName(status));
                return;
            }
        }

        buffer_fill = 0;
        char c;
        while(buffer_fill<16) {
            if(left>0) {
                if(_in_hex) {
                    byte v=0;
                    c = payload.as_cstr()[pos];
                    if(c>'A') {
                        if(c>'a') c -= 'a'-'A';
                        c -= 'A';
                        c += 10;
                        if(c>15) c=15;
                        v=((byte)c)<<4;
                    }
                    pos ++;
                    c = payload.as_cstr()[pos];
                    if(c>'A') {
                        if(c>'a') c -= 'a'-'A';
                        c -= 'A';
                        c += 10;
                        if(c>15) c=15;
                        v+=(byte)c;
                    }
                    pos ++;
                    c=(char)v; // convert to signed
                } else {
                    c = payload.as_cstr()[pos];
                    pos ++;
                }
                left --;
                buffer[buffer_fill] = c;
            } else {
                buffer[buffer_fill] = 0;
            }
            buffer_fill ++;
        }

        // Write block
        MFRC522::StatusCode status = sensor->MIFARE_Write(block, buffer, 16);
        if (status != MFRC522::STATUS_OK) {
            ulog(F("MIFARE_Write() failed: %d"),
                sensor->GetStatusCodeName(status));
            return;
        }
    }  // end while left

    if(_use_auth) {
        sensor->PICC_HaltA();
        sensor->PCD_StopCrypto1();
    } // TODO: also allow to select key B

}


Mfrc522::Mfrc522(const char* name, uint16_t data_size, 
        bool in_hex, bool use_auth, const byte* six_byte_key) : Device(name) {
    _use_auth = use_auth;
    if(_use_auth) {
        if(six_byte_key) {
            for (byte i=0; i<6; i++) 
                key.keyByte[i] = six_byte_key[i];
        } else {
            for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
        }
    }
    pollrate(250); // 4 times per s should be enough
    add_subdevice(new Subdevice("")); // data (0)
    add_subdevice(new Subdevice(F("uid"))); // uid (1)
    add_subdevice(new Subdevice(F("picc"))); // picc (2)
    add_subdevice(new Subdevice(F("set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            ulog(F("Mfrc522 set requested."));
            write(payload);
            return true;
        });
    _in_hex = in_hex;
    if(_in_hex)
        _data_size = min((uint16_t)(measured_value().max_length()/2)*2, data_size*2);
    else
        _data_size = min((uint16_t)measured_value().max_length(), data_size);
    block_count = (_data_size+15)/16;
    ulog(F("MFRC522 setup. hex: %d, data_size: %d, block_count: %d"),
        _in_hex, _data_size, block_count);
}


void Mfrc522::start() {
    ulog(F("Starting MFRC522 initialization."));
    // TODO: check on esp32
    sensor = new MFRC522(15, 0); // D8, D3 on Wemos
    if(sensor) {
        SPI.begin(); // Init SPI bus
        sensor->PCD_Init(); // Init MFRC522 card
        // just to be sure to read maximum distance (TODO: check if power issue?)
        sensor->PCD_SetAntennaGain(sensor->RxGain_max);
        _started = true;
        ulog(F("MFRC522 initialized."));
    } else {
        ulog(F("Trouble reserving memory for MFRC522."));
    }
}


bool Mfrc522::measure() {
    measured_value(0).clear(); // empty this by default TODO: check if necessary or always empty on start

    // only read when new card there
    if ( ! sensor->PICC_IsNewCardPresent()) {
        // no card, make sure to return empty value
        measured_value(1).from(F("NONE"));
        measured_value(2).from(F("NONE"));
        return true;
    }

    // Select one of the cards, TODO: handle multiple
    if ( ! sensor->PICC_ReadCardSerial()) {
        return false;
    }

    Ustring buf;

    for (byte i = 0; i < sensor->uid.size; i++) {
        buf.add_hex(sensor->uid.uidByte[i]);
    }
    measured_value(1).copy(buf);

    // PICC type
    MFRC522::PICC_Type piccType = sensor->PICC_GetType(sensor->uid.sak);
    measured_value(2).from((char*)sensor->PICC_GetTypeName(piccType));

    //sensor->PICC_DumpDetailsToSerial(&(sensor->uid)); //dump some details about the card

    //sensor->PICC_DumpToSerial(&(sensor->uid)); //uncomment this to see all blocks in hex


    //---- read data
    // TODO: make sure to use default keys for mifare classic and take care of 4 byte pages of MIFARE Ultralight

    if(_data_size<1) return true; // nothing to read

    buf.clear();

    byte buffer2[18];
    byte len;
    MFRC522::StatusCode status;

    for(uint16_t block = 1; block<block_count+1; block++) {

        if(_use_auth) {
            // authenticate
            MFRC522::StatusCode status = sensor->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
                    block, &key, &(sensor->uid)); //line 834
            if (status != MFRC522::STATUS_OK) {
                ulog(F("MFRC522 Authentication failed: %s"),
                    sensor->GetStatusCodeName(status));
                return true; // As we already read the id, we have some values to return
            }
        }

        len = 18; // start with full buffer
        status = sensor->MIFARE_Read(block, buffer2, &len);
        if (status != MFRC522::STATUS_OK) {
            ulog(F("MFRC522 Reading block %d failed: %s"), block, 
                sensor->GetStatusCodeName(status) );
            break;
        }

        ulog(F("Block len: %d"), len);

        for (uint8_t i = 0; i < len-2; i++) {
            if(_in_hex) {
                buf.add_hex(buffer2[i]);
            } else {
                if(buffer2[i]<32)
                    buf.add('.');
                else buf.add((char)buffer2[i]);
            }
        }
    }

    measured_value().copy(buf);

    if(_use_auth) {
        sensor->PICC_HaltA();
        sensor->PCD_StopCrypto1();
    } // TODO: also allow to select key B

    // End reading data
    return true;
}
