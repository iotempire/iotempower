// ulnoiot-arduino i2c connector to communicate with ulnoiot-node via i2c
//
// Based on Wire example by Nicholas Zambetti <http://www.zambetti.com>
//
// Author: ulno (http://ulno.net)
// Created: 2017-10-20
//
// Multimaster doesn't work as micropython doesn't support slave mode
// (Arduino multimaster decribed here http://forum.arduino.cc/index.php?topic=13579.0)
// Other option is to use https://github.com/todbot/SoftI2CMaster for alternative Master
// Settling now with a potential request for bus exclusivity

#ifndef UlnoiotI2c_h
#define UlnoiotI2c_h

#define IOTEMPOWER_I2C_ADDRESS 8 // Slave address to register as (don't chose something smaler than 8)

#include "Arduino.h"
#include <string.h>

// real buffer takes first two bytes as counter, 3rd as size, and 4th as time in ms request
#define IOTEMPOWER_I2C_BUFFER_SIZE 36 // < 50 (max what can be send in 1ms on 400 KHz)
#define IOTEMPOWER_I2C_REQUEST_DEFAULT 5 // Default request time if not specified
#define IOTEMPOWER_I2C_REQUEST_MAXBLOCK 1000 // max time to block for waiting for request confirmation

typedef void (*ulnoiot_i2c_receive_callback_type)(char *, int);

class UlnoiotI2c {
  public:
    UlnoiotI2c(int init_time, int addr, ulnoiot_i2c_receive_callback_type callback);
    UlnoiotI2c(int init_time, ulnoiot_i2c_receive_callback_type callback);
    UlnoiotI2c(int init_time, int addr);
    UlnoiotI2c(int init_time);
    void init(int init_time);
    void write(String s);
    void suspend( int timems );
    void request();
    void receive(int count);
  private:
    char buffer1[IOTEMPOWER_I2C_BUFFER_SIZE];
    char buffer2[IOTEMPOWER_I2C_BUFFER_SIZE];
    char *buffer;
    char receive_buffer[IOTEMPOWER_I2C_BUFFER_SIZE+1]; // one more which can be 0 for end
    int receive_buffer_size;
    int request_bus; // If this is > 0: request time in ms on bus when giving next answer
    bool request_confirmed; // This is set to true, when a request finishes and was set to false before
    int buffer_counter; // indicates new-ness of packages
    ulnoiot_i2c_receive_callback_type receive_callback;
    bool inwrite; // ulnoiot is in write critical region
    bool inrequest; // ulnoiot is in request

    void init(int init_time, int addr, ulnoiot_i2c_receive_callback_type callback);
};

#endif // UlnoiotI2c_h