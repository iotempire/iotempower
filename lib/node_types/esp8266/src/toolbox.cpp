// toolbox.cpp

#include <Arduino.h>
#include "toolbox.h"

bool Ustring::add(const Ustring& other) {
    int ownlen = length();
    int otherlen = other.length();
    bool untruncated = true;

    if( ownlen+otherlen > ULNOIOT_MAX_STRLEN ) {
        otherlen = ULNOIOT_MAX_STRLEN - ownlen;
        untruncated = false;
    }
    strncpy(cstr+ownlen, other.cstr, otherlen);
    return untruncated;
}

bool Ustring::from(const char* _cstr) {
    int otherlen = strnlen(_cstr,ULNOIOT_MAX_STRLEN+1);
    bool untruncated = true;
    if(otherlen > ULNOIOT_MAX_STRLEN) {
        otherlen = ULNOIOT_MAX_STRLEN;
        untruncated = false;
    }
    strncpy(cstr,_cstr,ULNOIOT_MAX_STRLEN);
    return untruncated;
}

bool Ustring::from(const byte* payload, unsigned long length) {
    bool untruncated = true;
    if(length > ULNOIOT_MAX_STRLEN) {
        length = ULNOIOT_MAX_STRLEN;
        untruncated = false;
    }
    strncpy(cstr, (const char *)payload, length); // TODO: think about proper decoding
    cstr[length] = 0; // terminate properly
    return untruncated;
}



void reboot() {
  // TODO: add network debugging
  Serial.println("Rebooting in 5 seconds.");
  delay(5000);
  // TODO: Consider counting in rtc and going into reconfigure after a while

  // make sure, reset is clean, gpio0 has to be high
  pinMode(0,OUTPUT);
  digitalWrite(0,1);
  yield();
  delay(10);
  yield();
  delay(500); // let things settle a bit
  ESP.restart(); // fails when serial activity TODO: solve or warn?
}

void controlled_crash(const char * error_message) {
    Serial.println();
    Serial.println("Crash, cause (reboot in 5s):");
    Serial.println(error_message);
    reboot();
}
