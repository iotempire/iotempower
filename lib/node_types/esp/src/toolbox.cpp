// toolbox.cpp

#include <Arduino.h>
#include "toolbox.h"

bool Ustring::add(const Ustring& other) {
    int ownlen = length();
    int otherlen = other.length();
    bool untruncated = true;

    if( ownlen+otherlen > IOTEMPOWER_MAX_STRLEN ) {
        otherlen = IOTEMPOWER_MAX_STRLEN - ownlen;
        untruncated = false;
    }
    strncpy(cstr+ownlen, other.cstr, otherlen+1); // +1 also copy 0
    case_adjust();
    return untruncated;
}

bool Ustring::add(const char* other) {
    int ownlen = length();
    int otherlen = strnlen(other, IOTEMPOWER_MAX_STRLEN-ownlen);
    bool untruncated = true;

    if( ownlen+otherlen > IOTEMPOWER_MAX_STRLEN ) {
        otherlen = IOTEMPOWER_MAX_STRLEN - ownlen;
        untruncated = false;
    }
    strncpy(cstr+ownlen, other, otherlen+1); // +1 also copy 0
    case_adjust();
    return untruncated;
}

bool Ustring::add(const __FlashStringHelper* other) {
    int ownlen = length();
    int otherlen = strnlen_P((PGM_P)other, IOTEMPOWER_MAX_STRLEN-ownlen);
    bool untruncated = true;

    if( ownlen+otherlen > IOTEMPOWER_MAX_STRLEN ) {
        otherlen = IOTEMPOWER_MAX_STRLEN - ownlen;
        untruncated = false;
    }
    strncpy_P(cstr+ownlen, (PGM_P)other, otherlen+1); // +1 also copy 0
    case_adjust();
    return untruncated;
}

bool Ustring::add(char c) {
    int ownlen = length();
    bool untruncated = true;

    if(ownlen < IOTEMPOWER_MAX_STRLEN) {
        cstr[ownlen] = c;
        cstr[ownlen+1] = 0; // 0 terminate correctly
    } else {
        untruncated = false;
    }
    case_adjust();
    return untruncated;
}

bool Ustring::add_hex(uint8_t c) {
    int ownlen = length();
    bool untruncated = true;
    uint8_t nib;

    if(ownlen < IOTEMPOWER_MAX_STRLEN-1) {
        nib = c>>4;
        cstr[ownlen] = nib<10?'0'+nib:'A'+nib-10;
        nib = c&15;
        cstr[ownlen+1] = nib<10?'0'+nib:'A'+nib-10;
        cstr[ownlen+2] = 0; // 0 terminate correctly
    } else {
        untruncated = false;
    }
    case_adjust();
    return untruncated;
}

bool Ustring::from(const char* _cstr) {
    int otherlen = strnlen(_cstr,IOTEMPOWER_MAX_STRLEN+1);
    bool untruncated = true;
    if(otherlen > IOTEMPOWER_MAX_STRLEN) {
        otherlen = IOTEMPOWER_MAX_STRLEN;
        untruncated = false;
    }
    strncpy(cstr,_cstr,IOTEMPOWER_MAX_STRLEN);
    case_adjust();
    return untruncated;
}

bool Ustring::from(const __FlashStringHelper* _cstr) {
    int otherlen = strnlen_P((PGM_P)_cstr,IOTEMPOWER_MAX_STRLEN+1);
    bool untruncated = true;
    if(otherlen > IOTEMPOWER_MAX_STRLEN) {
        otherlen = IOTEMPOWER_MAX_STRLEN;
        untruncated = false;
    }
    strncpy_P(cstr,(PGM_P)_cstr,IOTEMPOWER_MAX_STRLEN);
    case_adjust();
    return untruncated;
}

bool Ustring::from(const byte* payload, unsigned int length) {
    bool untruncated = true;
    if(length > IOTEMPOWER_MAX_STRLEN) {
        length = IOTEMPOWER_MAX_STRLEN;
        untruncated = false;
    }
    strncpy(cstr, (const char *)payload, length); // TODO: think about proper decoding
    cstr[length] = 0; // terminate properly
    case_adjust();
    return untruncated;
}

bool Ustring::remove(unsigned int from, unsigned int interval) {
    unsigned int mylen = length();
    if(from > mylen) return false;
    bool untruncated = true;
    if( from + interval > mylen ) {
        untruncated = false;
        interval = mylen - from;
    }
    memcpy(cstr + from, cstr + from + interval, mylen - from - interval);
    cstr[mylen - interval]=0;
    return untruncated;
}

int Ustring::compare(const char* other) const {
    int delta;
    if(_ignore_case) {
        for(int i=0; i<length(); i++) {
            delta = (int)tolower(cstr[i])-(int)tolower(other[i]);
            if(delta!=0) return delta;
        }
        return 0;
    } else {
        return strncmp(cstr, other, IOTEMPOWER_MAX_STRLEN);
    }
}

bool Ustring::equals(const char* other, bool ic) const {
    if(ic) {
        for(int i=0; i<length(); i++) {
            if(tolower(cstr[i])!=tolower(other[i])) return false;
        }
        return true;
    } else {
        return equals(other);
    }
}

void Ustring::strip() {
    int p;
    // truncate
    for(p=length()-1; p>=0; p--) {
        char c=cstr[p];
        if(c>32) break;
    }
    cstr[p+1]=0;
    // find first non whitespace
    for(p=0; p<length(); p++) {
        char c=cstr[p];
        if(c>32) break;
    }
    remove(0,p);
}

int Ustring::find(const char* pattern) {
    int p,i;
    int l=(int)length();
    for(p=0;p<length();p++) {
        for(i=0; i<l-p; i++) {
            if(pattern[i]==0) { // reached the end, so we found the pattern
                return p;
            }
            if(pattern[i]!=cstr[p+i]) {
                break; // try next
            }
        }
        if(p+i>=l) return -1; // no space to find left
    }
    return -1;
}

void Ustring::strip_param() {
    strip();
    int i;
    for(i=0; i<length(); i++) {
        if(cstr[i]<=32) break;
    }
    remove(0,i+1);
}

Ustring& Ustring::printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cstr, IOTEMPOWER_MAX_STRLEN, fmt, ap);
    va_end(ap);
    return *this;
}

int Ustring::scanf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int retval = vsscanf(cstr, fmt, ap);
    va_end(ap);
    return retval;
}



void reboot() {
    // TODO: add network debugging
    Serial.println(F("Rebooting in 5 seconds."));
    delay(5000);
    // TODO: Consider counting in rtc and going into reconfigure after a while

    // make sure, reset is clean, gpio0 has to be high
    pinMode(0,OUTPUT);
    digitalWrite(0,1);
    yield();
    delay(10);
    yield();
    delay(500); // let things settle a bit
    #if ESP32
        ESP.restart(); // fails when serial activity TODO: solve or warn?
    #else
        ESP.reset(); // fails when serial activity TODO: solve or warn?
    #endif
}

void controlled_crash(const char * error_message) {
    ulog(F("\nCrash, cause (reboot in 5s): %s"), error_message);
    reboot();
}

void ulog(const char *fmt, ...) {
    static bool serial_initialized=false;
    if(!serial_initialized) {
        serial_initialized = true;
        Serial.begin(115200);
        delay(500); // Wait for serial to get ready
        Serial.println();
        Serial.println();
        Serial.println(F("Serial ready."));
    }
	char buf[LOG_LINE_MAX_LEN];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, LOG_LINE_MAX_LEN, fmt, ap);
	va_end(ap);
	Serial.println(buf);
}

void ulog(const __FlashStringHelper *fmt, ...) {
    static bool serial_initialized=false;
    if(!serial_initialized) {
        serial_initialized = true;
        Serial.begin(115200);
        delay(500); // Wait for serial to get ready
        Serial.println();
        Serial.println();
        Serial.println(F("Serial ready."));
    }
	char buf[LOG_LINE_MAX_LEN];
	va_list ap;
	va_start(ap, (const char*)fmt);
	vsnprintf(buf, LOG_LINE_MAX_LEN, (const char*)fmt, ap);
	va_end(ap);
	Serial.println(buf);
}

long urandom(long from, long upto_exclusive) {
    return random(from, upto_exclusive);
}

const PROGMEM char* str_on = "on";
const PROGMEM char* str_off = "off";
