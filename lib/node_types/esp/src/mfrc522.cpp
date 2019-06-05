// mfrc522.cpp
#include "mfrc522.h"

// TODO: this might be just an i2c device, check if it can be defined as one

Mfrc522::Mfrc522(const char* name)
        : Device(name) {
    add_subdevice(new Subdevice("")); // data (0)
    add_subdevice(new Subdevice("uid")); // uid (1)
    add_subdevice(new Subdevice("picc")); // picc (2)
    add_subdevice(new Subdevice("set", true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // any load here triggers tare
            ulog("Mfrc522 set requested.");
            // TODO: implement
            return true;
        });
 
}


void Mfrc522::start() {
    ulog("Starting MFRC522 initialization.");
    // TODO: check on esp32
    sensor = new MFRC522(15, 0); // D8, D3 on Wemos
    if(sensor) {
        SPI.begin(); // Init SPI bus
        sensor->PCD_Init(); // Init MFRC522 card
        // just to be sure to read maximum distance (TODO: check if power issue?)
        sensor->PCD_SetAntennaGain(sensor->RxGain_max);
        _started = true;
        last_read = millis();
        ulog("MFRC522 initialized.");
    } else {
        ulog("Trouble reserving memory for MFRC522.");
    }
}


bool Mfrc522::measure() {
    unsigned long current = millis();
    unsigned long delta = current - last_read;
    bool measured = false;

    // only read when new card there
    if ( ! sensor->PICC_IsNewCardPresent()) {
        // no card, make sure to return empty value
        measured_value(0).clear();
        measured_value(1).from("NONE");
        measured_value(2).from("NONE");
        return true;
    }

    // Select one of the cards, TODO: handle multiple
    if ( ! sensor->PICC_ReadCardSerial()) {
        return false;
    }

    Ustring buf;

    Serial.println("Buf:");
    Serial.println(buf.as_cstr());

    for (byte i = 0; i < sensor->uid.size; i++) {
        buf.add_hex(sensor->uid.uidByte[i]);
    }
    measured_value(1).copy(buf);

    // PICC type
    MFRC522::PICC_Type piccType = sensor->PICC_GetType(sensor->uid.sak);
    measured_value(2).from((char*)sensor->PICC_GetTypeName(piccType));

//    sensor->PICC_DumpDetailsToSerial(&(sensor->uid)); //dump some details about the card

    //sensor->PICC_DumpToSerial(&(sensor->uid)); //uncomment this to see all blocks in hex

    // TODO: read data

    //---------------------------------------- GET LAST NAME (Block 1)

    byte buffer2[18];
    block = 1;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    status = mfrc522.MIFARE_Read(block, buffer2, &len);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Reading failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    //PRINT LAST NAME
    for (uint8_t i = 0; i < 16; i++) {
        Serial.write(buffer2[i] );
    }


    //----------------------------------------

    Serial.println(F("\n**End Reading**\n"));

    delay(1000); //change value if you want to read cards faster

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // End reading data

    measured = true;
    last_read = current;
    return measured; // no new value available
}
