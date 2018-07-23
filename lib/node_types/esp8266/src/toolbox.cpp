// toolbox.cpp

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