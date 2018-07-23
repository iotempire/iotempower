// toolbox.h
// Small toolbox for ulnoiot on Arduino-like devices
//

#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include <cstring.h>
#include <stdio.h>
#include <stdlib.h>
#include "ulnoiot-default.h"

typedef etl::string<10> String;

// a simple class for handling fixed-length strings. Ustring stands for
// Ulnoiot-String
class Ustring: {
    protected:
        char cstr[ULNOIOT_MAX_STRLEN+1];
    public:
        void clear() {
            // make it empty and always terminated
            cstr[0]=0;
            cstr[ULNOIOT_MAX_STRLEN]=0;
        }
        int length() const { return strnlen(cstr,ULNOIOT_MAX_STRLEN); }
        int max_length() const { return ULNOIOT_MAX_STRLEN; }
        // most functions return true, when successful and false if something
        // had to be truncated
        bool from(int i) { 
            return snprintf(cstr, ULNOIOT_MAX_STRLEN+1, "%d", i) <= ULNOIOT_MAX_STRLEN; 
        }
        bool from(long i) { 
            return snprintf(cstr, ULNOIOT_MAX_STRLEN+1, "%ld", i) <= ULNOIOT_MAX_STRLEN; 
        }
        bool from(float f) { 
            // TODO: check if float is actually now supported in sprintf
            return snprintf(cstr, ULNOIOT_MAX_STRLEN+1, "%f", f) <= ULNOIOT_MAX_STRLEN; 
        }
        bool from(double f) { 
            return snprintf(cstr, ULNOIOT_MAX_STRLEN+1, "%f", f) <= ULNOIOT_MAX_STRLEN; 
        }
        bool from(const char* _cstr);
        bool from(const Ustring& other) { 
            strncpy(cstr,other.cstr,ULNOIOT_MAX_STRLEN);
            return true;
        }
        bool copy(const Ustring& other) { return from(other); }
        bool add(const Ustring& other);
        int compare(const Ustring& other) const {
            return strncmp(cstr, other.cstr, ULNOIOT_MAX_STRLEN);
        }
        bool equals(const Ustring& other) const { return compare(other) == 0; }
        bool empty() const { return cstr[0] == 0; }
        
        //int as_int() const { return atoi(cstr); }
        long as_int() const { return atol(cstr); }
        //float as_float() const { return atof(cstr); }
        double as_float() const { return atof(cstr); }

        const char* as_cstr() const { return cstr; }
        
        Ustring() { clear (); }
        Ustring(const char *initstr) {
            clear();
            strncpy(cstr,initstr,ULNOIOT_MAX_STRLEN);
        }
        Ustring(int i) { clear(); from(i); }
        Ustring(long i) { clear(); from(i); }
        Ustring(float f) { clear(); from(f); }
        Ustring(double f) { clear(); from(f); }
};

#endif // _TOOLBOX_H_