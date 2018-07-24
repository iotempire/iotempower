// toolbox.h
// Small toolbox for ulnoiot on Arduino-like devices
//

#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include "ulnoiot-default.h"

// a simple class for handling fixed-length strings. Ustring stands for
// Ulnoiot-String
class Ustring {
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
        bool from(const byte* payload, unsigned long length);
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

// small fixed size map with linear search functionality
// VALUE_TYPE needs to have a member function key, returning a Ustring,
// which is used for searching
// TODO: sort
template<class VALUE_TYPE, size_t SIZE>  
class Fixed_Map {
    private:
        VALUE_TYPE* list[SIZE];
        unsigned long count=0;
        unsigned long find_index(const Ustring& searchterm) {
            for(unsigned long i=0; i<SIZE; i++) {
                if(searchterm.equals(list[i]->key()))
                    return i;
            }
            return -1;
        }
    public:
        bool add(VALUE_TYPE* element) {
            if(count>=SIZE) {
                count = SIZE;
                return false;
            } else {
                list[count] = element;
                count ++;
            }
            return true;
        }
        VALUE_TYPE* find(const Ustring& searchterm) {
            unsigned long index = find_index(searchterm);
            if(index>0) {
                return list[index];
            }
            return NULL;
        }
        VALUE_TYPE* find(const char* searchterm) {
            Ustring usearch(searchterm);
            unsigned long index = find_index(usearch);
            if(index>0) {
                return list[index];
            }
            return NULL;
        }
        VALUE_TYPE* get(unsigned long index) {
            if(index>=count) return NULL;
            return list[index];
        }
        VALUE_TYPE* first() {
            return get(0);
        }
        unsigned long length() { return count;}
        // Do for all elements (can be aborted if the given func returns false)
        // this leads to for_each returning false. 
        bool for_each(std::function<bool (VALUE_TYPE&)> func) {
            for(unsigned long i=0; i<count; i++) {
                if(!func(*(list[i]))) 
                    return false;
            }
            return true;
        }
};

void reboot();

void controlled_crash(const char * error_message);

#endif // _TOOLBOX_H_