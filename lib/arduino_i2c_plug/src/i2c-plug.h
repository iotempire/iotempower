// IoTempower-Arduino i2c connector plug to communicate with
// an IoTempower-node via i2c
//
// _I2C_Plug.h
//
// Based on Wire example by Nicholas Zambetti <http://www.zambetti.com>
//
// Author: ulno (http://ulno.net)
// Created: 2017-10-20
//
// The corresponding device in IoTempower is an I2C_Socket
// (or i2c_socket as command)
//
// Multimaster isn't supported
// (Arduino multimaster described here
// http://forum.arduino.cc/index.php?topic=13579.0)
// Other option is to use https://github.com/todbot/SoftI2CMaster for alternative Master
// Settling now with a potential request for bus exclusivity

#ifndef _I2C_Plug_h
#define _I2C_Plug_h

// Slave address to register as (don't chose something smaller than 8)
// 8 is usually the address of the master
#define I2C_CONNECTOR_DEFAULT_ADDRESS 9

#include "Arduino.h"
#include <string.h>

// real buffer takes first two bytes as counter,
// 3rd as size (or a 0 as request to send current available data)
#define I2C_CONNECTOR_BUFFER_SIZE 35 // < 50 (max what can be send in 1ms on 400 KHz)
#define I2C_CONNECTOR_REQUEST_DEFAULT 5 // Default request time if not specified
#define I2C_CONNECTOR_REQUEST_MAXBLOCK 1000 // max time to block for waiting for request confirmation // TODO: check if used

typedef void (*i2c_plug_receive_callback_type)(char *, int);

class _I2C_Plug {
  private:
    char* buffer; // pointer for double buffering to one of the two previous
    char buffer1[I2C_CONNECTOR_BUFFER_SIZE]; // first buffer
    char buffer2[I2C_CONNECTOR_BUFFER_SIZE]; // second buffer
    char receive_buffer[I2C_CONNECTOR_BUFFER_SIZE+1]; // receive buffer (one more which can be 0 for end termination)
    int receive_buffer_size; // current fill of the receive buffer
    // bool request_confirmed; // This is set to true, when a request finishes and was set to false before
    uint16_t buffer_counter; // indicates new-ness of packages
    i2c_plug_receive_callback_type receive_callback;
    bool inwrite; // in write critical region
    bool inrequest; // in request critical region
    int _addr; // own I2C address

    void init(int addr, i2c_plug_receive_callback_type callback);
    void init_i2c();

  public:
    _I2C_Plug() {};
    void begin(int addr, i2c_plug_receive_callback_type callback);
    void begin(i2c_plug_receive_callback_type callback);
    void begin(int addr=I2C_CONNECTOR_DEFAULT_ADDRESS);
    bool write(String s); // return true when write successful (only fills buffer)
    // void suspend( int timems );
    void send_data_after_request(); // this actually sends available data out 
    void receive(int count);
};

extern _I2C_Plug I2C_Plug;

#endif // _I2C_Plug_h