// toolbox.h
// Small toolbox for iotempower on Arduino-like devices
//

#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include <Arduino.h>
// no special logging for esp8266
#if defined(ESP32)
    #include <esp_log.h>
#endif

#if defined(ESP32)
    // PROGMEM and F-Strings seem to be utterly broken in new versions of arduino esp32 framework
    // TODO - remove this after fix found
    #ifdef F
        #undef F
        #define F(param) param
    #endif
#endif

#include "iotempower-default.h"

extern const PROGMEM char* str_on;
extern const PROGMEM char* str_off;
extern const PROGMEM char* str_empty;
extern const PROGMEM char* str_set;

// Logging
void ulog_serial_disable();
void ulog_serial_enable();
void ulog_internal();
void ulog_internal(const char *fmt, ...);
void ulog_internal(const __FlashStringHelper *fmt, ...);
// TODO: work on making logging more configurable
#define ULOG_FORMAT  F("[%6u][%s:%u] %s(): "), (unsigned long) (esp_timer_get_time() / 1000ULL), pathToFileName(__FILE__), __LINE__, __FUNCTION__
#ifdef ESP32
//    #define ulog(fmt, ...) log_i(__VA_OPT__(fmt) "%s" __VA_OPT__(, __VA_ARGS__))
//    #define ulog(fmt, ...) log_i(__VA_OPT__(fmt) __VA_OPT__(, __VA_ARGS__));
//    #define ulog(fmt, ...) ESP_LOGI("IoT", fmt, ##__VA_ARGS__);
    #define ulog(fmt, ...) log_printf(ULOG_FORMAT); log_printf(fmt, ##__VA_ARGS__); log_printf("\n")
#else
//    #define ulog(fmt, ...) log_printf(ULOG_FORMAT); log_printf(fmt, ##__VA_ARGS__); log_printf("\n")
    #define ulog(fmt, ...) ulog_internal(fmt, ##__VA_ARGS__)
#endif

// a simple class for handling fixed-length strings.
// Ustring stands for UlnoIoT (old framework name) - String
class Ustring {
    protected:
        char cstr[IOTEMPOWER_MAX_STRLEN+1];
        bool _ignore_case = false;
    public:
        void clear() {
            // make it empty and always terminated
            cstr[0]=0;
            cstr[IOTEMPOWER_MAX_STRLEN]=0;
        }

        // most functions return true, when successful and false if something
        // had to be truncated

        /* remove up to "length" bytes starting from position "from" */
        bool remove(unsigned int from, unsigned int length);
        bool remove(unsigned int length) {
            return remove(0,length);
        }
        int length() const { return strnlen(cstr,IOTEMPOWER_MAX_STRLEN); }
        int max_length() const { return IOTEMPOWER_MAX_STRLEN; }
        bool from(int i) { 
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%d", i) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(long i) { 
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%ld", i) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(unsigned int i) { 
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%u", i) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(unsigned long i) { 
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%lu", i) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(float f) { 
            // TODO: check if float is actually now supported in sprintf
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%f", f) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(double f) { 
            return snprintf(cstr, IOTEMPOWER_MAX_STRLEN+1, "%f", f) <= IOTEMPOWER_MAX_STRLEN; 
        }
        bool from(const char* _cstr);
        bool from(const __FlashStringHelper* _cstr);
        bool from(const byte* payload, unsigned int length);
        bool from(const char* payload, unsigned int length) {
            return from((byte*) payload, length);
        }
        /* ISO C++ says that these are ambiguous */
        /* really!?! , so use copy - not from */
        bool copy(const Ustring& other) { 
            strncpy(cstr,other.cstr,IOTEMPOWER_MAX_STRLEN);
            case_adjust();
            return true;
        }
        bool copy(const Ustring& other, int start, int len) {
            if(start<other.length()) {
                return from(other.as_cstr()+start,len);
            }
            return false;
        }
        bool copy(const Ustring& other, int len) {
            return from(other.as_cstr(), len);
        }
        bool add(const Ustring& other);
        bool add(const char* other);
        bool add(const __FlashStringHelper* other);
        bool add(char c);
        bool add_hex(uint8_t c);

        int compare(const char* other) const;
        int compare(const Ustring& other) const { return compare(other.cstr); }
        int compare(const __FlashStringHelper* other) const {
            Ustring us;
            us.from(other);
            return compare(us);
        };

        bool equals(const Ustring& other) const { return compare(other) == 0; }
        bool equals(const char* other) const { return compare(other) == 0; }
        bool equals(const __FlashStringHelper* other) const { return compare(other) == 0; }

        bool equals(const char* other, bool ignore_case) const;
        bool equals(const __FlashStringHelper* other, bool ignore_case) const {
            Ustring us;
            us.from(other);
            return equals(us.as_cstr(), ignore_case);
        };

        bool empty() const { return cstr[0] == 0; }
        
        //int as_int() const { return atoi(cstr); }
        long as_int() const { return atol(cstr); }
        //float as_float() const { return atof(cstr); }
        double as_float() const { return atof(cstr); }

        const char* as_cstr() const { return cstr; }

        void lower() {
            char *p = cstr;
            for ( ; *p; ++p) *p = tolower(*p);
        }

        void upper() {
            char *p = cstr;
            for ( ; *p; ++p) *p = toupper(*p);
        }

        void case_adjust() {
            if(_ignore_case) lower();
        }

        void strip();
        
        // strip until next blank (including that blank) if it exists
        void strip_param();

        int find(const char* pattern);
        int find(const __FlashStringHelper* pattern) {
            Ustring pattern_u;
            pattern_u.from(pattern);
            return find(pattern_u.as_cstr());
        };

        bool contains(const char* pattern) {
            return find(pattern) >= 0;
        }
        bool contains(const __FlashStringHelper* pattern) {
            return find(pattern) >= 0;
        }

        bool starts_with(const char* pattern) const {
            unsigned long len = strnlen(pattern, IOTEMPOWER_MAX_STRLEN);
            return memcmp(cstr,pattern,len) == 0;
        }

        bool starts_with(const __FlashStringHelper* pattern) const {
            unsigned long len = strnlen_P((PGM_P)pattern, IOTEMPOWER_MAX_STRLEN);
            return memcmp_P(cstr,pattern,len) == 0;
        }

        Ustring() { clear (); }
        Ustring(const char *initstr) {
            clear();
            strncpy(cstr,initstr,IOTEMPOWER_MAX_STRLEN);
            case_adjust();
        }
        Ustring(const __FlashStringHelper* initstr) {
            from(initstr);
        }
        Ustring(int i) { clear(); from(i); case_adjust();}
        Ustring(long i) { clear(); from(i); }
        Ustring(float f) { clear(); from(f); }
        Ustring(double f) { clear(); from(f); }
        Ustring(const char* payload, unsigned int length) { 
            clear();
            from(payload, length);
        }
        Ustring(const byte* payload, unsigned int length) { 
            clear();
            from(payload, length);
        }
        Ustring& ignore_case(bool ic) {
            _ignore_case = ic;
            case_adjust();
            return *this;
        }
        Ustring& ignore_case() {
            return ignore_case(true);
        }

        Ustring& printf(const char *fmt, ...);
        int scanf(const char *fmt, ...);
};

// Similar to Ustring, a fixed length buffer
class Fixed_Buffer {
    protected:
        byte _buffer[IOTEMPOWER_MAX_BUFLEN];
        unsigned long len = 0;
    public:
        const byte* buffer() {
            return _buffer;
        }
        void clear() {
            len=0;
        }
        unsigned long length() {
            return len;
        }
        // append a single byte
        void append_byte(byte b) {
            if(len>=IOTEMPOWER_MAX_BUFLEN) return;
            _buffer[len] = b;
            len ++;
        }
        // append without adding length, stop when buffer end reached
        void append_nolen(byte buflen, const byte* buf) {
            while(len < IOTEMPOWER_MAX_BUFLEN && buflen > 0) {
                _buffer[len] = *buf;
                buf ++;
                len ++;
                buflen --;
            }
        }
        // append with adding the len as a byte
        void append(byte buflen, const byte* buf) {
            append_byte(buflen);
            append_nolen(buflen, buf);
        }
        void skip(unsigned long bytes) {
            len = len+bytes;
            if(len>IOTEMPOWER_MAX_BUFLEN) len=IOTEMPOWER_MAX_BUFLEN;
        }
};


// small fixed size map with linear search functionality
// VALUE_TYPE needs to have a member function key(), returning a Ustring,
// which is used for searching
// TODO: sort
template<class VALUE_TYPE, size_t SIZE>  
class Fixed_Map {
    private:
        VALUE_TYPE* list[SIZE];
        unsigned int count;
        unsigned int find_index(const Ustring& searchterm) {
            for(unsigned int i=0; i<SIZE; i++) {
                if(searchterm.equals(list[i]->key()))
                    return i;
            }
            return -1;
        }
    public:
        // __attribute__((constructor(65535))) 
        Fixed_Map() {
            ulog(F("Fixed_Map constructor: count set to 0."));
            count=0;
        }
        bool add(VALUE_TYPE* element) {
            if(count >= SIZE) {
                count = SIZE;
                return false;
            } else {
                list[count] = element;
                count ++;
            }
            return true;
        }
        VALUE_TYPE* find(const Ustring& searchterm) {
            unsigned int index = find_index(searchterm);
            if(index>0) {
                return list[index];
            }
            return NULL;
        }
        VALUE_TYPE* find(const char* searchterm) {
            Ustring usearch(searchterm);
            unsigned int index = find_index(usearch);
            if(index>0) {
                return list[index];
            }
            return NULL;
        }
        VALUE_TYPE* get(unsigned int index) {
            if(index>=count) return NULL;
            return list[index];
        }
        VALUE_TYPE* first() {
            return get(0);
        }
        unsigned int length() { return count;}
        // Do for all elements (can be aborted if the given func returns false)
        // this leads to for_each returning false. 
        bool for_each(std::function<bool (VALUE_TYPE&)> func) {
            for(unsigned int i=0; i<count; i++) {
                if(!func(*(list[i]))) 
                    return false;
            }
            return true;
        }
};

void reboot();

void controlled_crash(const char * error_message);

long urandom(long from, long upto_exclusive);

bool is_serial_initialized();

void initialize_serial();

// constrain or limit a number to an interval
#define limit(nr, min, max) \
    ( (nr) < (min) ? (min):((nr) > (max) ? (max):(nr)) )


#endif // _TOOLBOX_H_