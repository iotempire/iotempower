// iotempower-arduino i2c connector to communicate with iotempower-node via i2c
//
// Based on Wire example by Nicholas Zambetti <http://www.zambetti.com>
//
// Author: ulno (http://ulno.net)
// Created: 2017-10-20
//
// Multimaster doesn't work as IoTempire nodes don't (yet) support slave mode
// (Arduino multimaster decribed here http://forum.arduino.cc/index.php?topic=13579.0)
// Other option is to use https://github.com/todbot/SoftI2CMaster for alternative Master
// Settling now with a potential request for bus exclusivity

#ifndef I2cConnector_h
#define I2cConnector_h

// Slave address to register as (don't chose something smaller than 8)
#define I2C_CONNECTOR_DEFAULT_ADDRESS 8 

#include "Arduino.h"
#include <string.h>

// real buffer takes first two bytes as counter,
// 3rd as size (or a 0 as request to send current available data)
#define I2C_CONNECTOR_BUFFER_SIZE 35 // < 50 (max what can be send in 1ms on 400 KHz)
#define I2C_CONNECTOR_REQUEST_DEFAULT 5 // Default request time if not specified
#define I2C_CONNECTOR_REQUEST_MAXBLOCK 1000 // max time to block for waiting for request confirmation // TODO: check if used

typedef void (*i2c_connector_receive_callback_type)(char *, int);

class I2cConnector {
  public:
    I2cConnector(int init_time, int addr, i2c_connector_receive_callback_type callback);
    I2cConnector(int init_time, i2c_connector_receive_callback_type callback);
    I2cConnector(int init_time, int addr=I2C_CONNECTOR_DEFAULT_ADDRESS);
    void init(int init_time);
    bool write(String s); // return true when write successful (only fills buffer)
    void suspend( int timems );
    void send_data_after_request(); // this actually sends available data out 
    void receive(int count);
  private:
    char buffer1[I2C_CONNECTOR_BUFFER_SIZE];
    char buffer2[I2C_CONNECTOR_BUFFER_SIZE];
    char *buffer;
    char receive_buffer[I2C_CONNECTOR_BUFFER_SIZE+1]; // one more which can be 0 for end
    int receive_buffer_size;
    int request_bus; // If this is > 0: request time in ms on bus when giving next answer
    bool request_confirmed; // This is set to true, when a request finishes and was set to false before
    int buffer_counter; // indicates new-ness of packages
    i2c_connector_receive_callback_type receive_callback;
    bool inwrite; // iotempower is in write critical region
    bool inrequest; // iotempower is in request
    int _addr;

    void init(int init_time, int addr, i2c_connector_receive_callback_type callback);
};

#endif // I2cConnector_h