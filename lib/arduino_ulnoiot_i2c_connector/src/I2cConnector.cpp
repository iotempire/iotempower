// iotempower-arduino i2c connector to communicate with iotempower-node via i2c
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

#include "I2cConnector.h"
#include "Arduino.h"
#include <string.h>
#include <Wire.h>


void I2cConnector::init(int init_time, int addr, i2c_connector_receive_callback_type callback) {
    buffer = buffer1;
    receive_buffer_size = 0;
    request_bus = 0;
    request_confirmed = true;
    buffer_counter = 1024;
    receive_callback = callback;
    inwrite = false;
    inrequest = false;
    _addr = addr;
    init(init_time);
}

I2cConnector::I2cConnector(int init_time, int addr, i2c_connector_receive_callback_type callback) {
    init(init_time, addr, callback);
}

I2cConnector::I2cConnector(int init_time, int addr) {
    init(init_time, addr, NULL);
}

I2cConnector::I2cConnector(int init_time, i2c_connector_receive_callback_type callback) {
    init(init_time, I2C_CONNECTOR_DEFAULT_ADDRESS, callback);
}

void I2cConnector::send_data_after_request() {
    // no Serial.prints in this method allowed
    // using Serial here messes up all the comunication (also if done beforehand)

    char* buf = buffer; // pick one buffer
    if(inrequest) {
        Serial.println("Double request, returning.");
        return;
    }
    inrequest=true;
    Wire.write(buf, ((unsigned int)buf[2])+3); // respond
    //Wire.write(buf, I2C_CONNECTOR_BUFFER_SIZE); // respond
    //Wire.write("!!!!!!!!!! !!!!!!!!!! !!!!!!!!!!    ", 35); // respond
    //Wire.write("hell2\n");
    
    // makes it crash: noInterrupts(); TODO: check if next line is still atomic
    if (!request_confirmed) request_confirmed = true;
    // makes it crash: interrupts();
    inrequest=false;
}

bool I2cConnector::write(String s) {
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
    if( buffer_counter >= 0xffff ) buffer_counter = 0; // prevent 0xffff
    for( int i=0; i<l; i++ ) buf[i+3] = s[i]; // copy contents
    // set counter
    buf[0] = (buffer_counter & 0xff00 ) >> 8;
    buf[1] = buffer_counter & 0xff;
    buf[2] = l;  // set length
    //buf[3] = request_bus; // if given, request timeout for local master
    request_bus=0; // reset again
    buffer =  buf; // swap to updated buffer
    written = true;
    inwrite = false; // leave critical region
  }
  interrupts();
  Serial.println("Updated buffer.");
  return written; // we still don't know if this value was received, it just made it into the out-buffer
}

// # define I2C_CONNECTOR_END_DELAY 10
// void I2cConnector::suspend( int timems ) {
//   timems += I2C_CONNECTOR_END_DELAY;
//   // scale down time
//   unsigned int t=0;
//   if( timems<0 ) timems=0;
//   else if( timems>5000 ) timems = 5000; // max 5s -> TODO: move to constants
//   if( timems>=100 ) {
//     t+=128; // set high bit
//     timems = (timems+50) / 100;
//   }
//   t += timems;
//   // Wait/block until new request has been sent
//   noInterrupts();
//   request_confirmed = false;
//   request_bus = t;
//   interrupts();
//   // block until request came in but max maxblock
//   int counter = 0;
//   // Arduino's compiler just doesn't allow to read the request confirmed
//   bool request_copy;
//   while (true) {
//     noInterrupts(); // if interrupt clause not given, it is just cached and never read
//     request_copy = request_confirmed;
//     interrupts();
//     if (request_copy) break;
//     delayMicroseconds(1000);
//     counter += 1;
//     if (counter >= I2C_CONNECTOR_REQUEST_MAXBLOCK ) {
//       Serial.println("iotempower i2c maxblock reached, continuing");
//       break;
//     }
//   }
//   delayMicroseconds(1000*I2C_CONNECTOR_END_DELAY);
//   /*Serial.print("Request confirmed: ");
//   Serial.print(request_confirmed);
//   Serial.print(" counter: ");
//   Serial.println(counter);  // debug */
// }

void I2cConnector::receive(int count) {
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

// That's pretty dirty and makes this a singleton. TODO: think if this could be done nicer.
I2cConnector *_i2c_connector_last_init; // break out of objects into static environment
void _i2c_connector_request_caller() {
    
    // no Serial.prints in this method allowed
    // using Serial here messes up all the comunication (also if done beforehand)

    // Wire.write(0);
    // Wire.write(41);
    // Wire.write(6);
    // Wire.write("hell2\n");
    
    _i2c_connector_last_init->send_data_after_request();
    // char* buf = _i2c_connector_last_init->buffer; // pick one buffer

    // Wire.write(buf, ((unsigned int)buf[2])+3); // respond
    
}

void _i2c_connector_receive_caller(int count) {
    _i2c_connector_last_init->receive(count);
}

void I2cConnector::init(int init_time) {
  Wire.begin(_addr); // join i2c bus as slave with respective address
  //Wire.setClock(100000); // TODO: check if necessary
  // disable pull-ups
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);
  _i2c_connector_last_init = this;
  Wire.onRequest(_i2c_connector_request_caller); // register request function (to send data to master)
  Wire.onReceive(_i2c_connector_receive_caller); // register receive function (to receive data from master)
  write(""); // init empty buffer
  delay(100); // let things settle TODO: check if necessary this long
  //suspend(init_time); // wait given time
}

/*void i2c_connector_reinit() {
  Wire.onRequest(i2c_connector_request); // register request event (to send data to master)
  Wire.onReceive(i2c_connector_receive); // register receive event (to receive data from master)
}

void i2c_connector_deinit() {
  Wire.onRequest(NULL); // de-register request event
  Wire.onReceive(NULL); // de-register receive event
}*/
