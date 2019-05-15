// IoTempower-Arduino i2c connector plug to communicate with
// _I2C_Plug.cpp

#include "Arduino.h"
#include <string.h>
#include <Wire.h>
#include "i2c-plug.h"

// That's pretty dirty and makes this a singleton. TODO: think if this could be done nicer.
_I2C_Plug *_i2c_plug_last_init; // break out of objects into static environment
void _i2c_plug_request_caller() {
    
    // no Serial.prints in this method allowed
    // using Serial here messes up all the comunication (also if done beforehand)

    // go back into the class
    _i2c_plug_last_init->send_data_after_request();
}

void _i2c_plug_receive_caller(int count) {
    _i2c_plug_last_init->receive(count);
}

void _I2C_Plug::init_i2c() {
    Wire.begin(_addr); // join i2c bus as slave with respective address
    //Wire.setClock(100000); // TODO: check if necessary
    // disable pull-ups
    pinMode(SDA, INPUT);
    pinMode(SCL, INPUT);
    _i2c_plug_last_init = this;
    Wire.onRequest(_i2c_plug_request_caller); // register request function (to send data to master)
    Wire.onReceive(_i2c_plug_receive_caller); // register receive function (to receive data from master)
    write(""); // init empty buffer
    delay(500); // let things settle TODO: check if necessary this long
}


void _I2C_Plug::init(int addr, i2c_plug_receive_callback_type callback) {
    buffer = buffer1;
    receive_buffer_size = 0;
//    request_confirmed = true;
    buffer_counter = 0;
    receive_callback = callback;
    inwrite = false;
    inrequest = false;
    _addr = addr;
    init_i2c();
}


void _I2C_Plug::begin(int addr, i2c_plug_receive_callback_type callback) {
    init(addr, callback);
}


void _I2C_Plug::begin(int addr) {
    init(addr, NULL);
}


void _I2C_Plug::begin(i2c_plug_receive_callback_type callback) {
    init(I2C_CONNECTOR_DEFAULT_ADDRESS, callback);
}


void _I2C_Plug::send_data_after_request() {
    // no Serial.prints in this method allowed
    // using Serial here messes up all the comunication (also if done beforehand)

    // testing
    // Wire.write(0);
    // Wire.write(41);
    // Wire.write(6);
    // Wire.write("hell2\n");

    char* buf = buffer; // pick one buffer
    if(inrequest) {
        // no serial!!  TODO: debug elsewhere via error flag Serial.println("Double request, returning.");
        return;
    }
    inrequest=true;
    Wire.write(buf, ((unsigned int)buf[2])+3); // respond
    //Wire.write(buf, I2C_CONNECTOR_BUFFER_SIZE); // respond
    //Wire.write("!!!!!!!!!! !!!!!!!!!! !!!!!!!!!!    ", 35); // respond
    //Wire.write("hell2\n");
    
    // makes it crash: noInterrupts(); interrupts seem blocked -> Serial problems
    // if (!request_confirmed) request_confirmed = true;
    // makes it crash: interrupts();
    inrequest=false;
}


bool _I2C_Plug::write(String s) {
    char *buf;
    bool written = false;

    // problem is that this function could be interrupted by the request
    // therefore, we use double buffering.
    noInterrupts(); // this needs to be written atomically
    if (!inwrite) { // enter only once
        inwrite=true; // enter critical region
        
        if( buffer == buffer1 ) { // write to inactive buffer
            buf = buffer2;
        } else {
            buf = buffer1;
        }
        int l = s.length();
        if(l > I2C_CONNECTOR_BUFFER_SIZE - 3) { // ignore if string too long
            l = I2C_CONNECTOR_BUFFER_SIZE - 3;
        }
        buffer_counter += 1;
        if(buffer_counter >= 0xffff) buffer_counter = 0; // prevent 0xffff
        for(int i=0; i<l; i++) buf[i+3] = s[i]; // copy contents
        // set counter
        buf[0] = (buffer_counter & 0xff00 ) >> 8;
        buf[1] = buffer_counter & 0xff;
        buf[2] = l;  // set length
        buffer =  buf; // swap to updated buffer
        written = true;
        inwrite = false; // leave critical region
    }
    interrupts();
    return written; // we still don't know if this value was received, it just made it into the out-buffer
}


void _I2C_Plug::receive(int count) {
    receive_buffer_size = 0;
    while(Wire.available()) { // loop through all
        if( receive_buffer_size >= I2C_CONNECTOR_BUFFER_SIZE ) break;
        receive_buffer[receive_buffer_size] = Wire.read();
        receive_buffer_size ++;
    }
    receive_buffer[receive_buffer_size] = 0; // 0-terminate
    if(receive_callback) {
        receive_callback( receive_buffer, receive_buffer_size );
    }
}


/*void i2c_plug_reinit() {
  Wire.onRequest(i2c_plug_request); // register request event (to send data to master)
  Wire.onReceive(i2c_plug_receive); // register receive event (to receive data from master)
}

void i2c_plug_deinit() {
  Wire.onRequest(NULL); // de-register request event
  Wire.onReceive(NULL); // de-register receive event
}*/

_I2C_Plug I2C_Plug;
