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

#include "UlnoiotI2c.h"
#include "Arduino.h"
#include <string.h>
#include <Wire.h>


void UlnoiotI2c::init(int init_time, int addr, ulnoiot_i2c_receive_callback_type callback) {
  buffer = buffer1;
  receive_buffer_size = 0;
  request_bus = 0;
  request_confirmed = true;
  buffer_counter = 0;
  ulnoiot_i2c_receive_callback_type receive_callback = callback;
  inwrite = false;
  inrequest = false;
  addr = addr;
  init(init_time);
}

UlnoiotI2c::UlnoiotI2c(int init_time, int addr, ulnoiot_i2c_receive_callback_type callback) {
  init(init_time, addr, callback);
}

UlnoiotI2c::UlnoiotI2c(int init_time, int addr) {
  init(init_time, addr, NULL);
}

UlnoiotI2c::UlnoiotI2c(int init_time) {
  init(init_time, ULNOIOT_I2C_ADDRESS, NULL);
}

UlnoiotI2c::UlnoiotI2c(int init_time, ulnoiot_i2c_receive_callback_type callback) {
  init(init_time, ULNOIOT_I2C_ADDRESS, callback);
}

void UlnoiotI2c::request() {
  char *buf = buffer; // pick one buffer
  inrequest=true;
  Wire.write(buf, ((unsigned int)buf[2])+4); // respond
  // makes it crash: noInterrupts(); TODO: check if next line is still atomic
  if (!request_confirmed) request_confirmed = true;
  // makes it crash: interrupts();
  inrequest=false;
//  Serial.print("Wrote response. Confirmed: ");
//  Serial.println(request_confirmed);
}

void UlnoiotI2c::write(String s) {
  char *buf;

  // problem is that this function could interrupt the request
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
    if(l > ULNOIOT_I2C_BUFFER_SIZE - 4) { // ignore if string too long
      l = ULNOIOT_I2C_BUFFER_SIZE - 4;
    }
    buffer_counter += 1;
    if( buffer_counter >= 0xffff ) buffer_counter = 0; // prevent 0xffff
    for( int i=0; i<l; i++ ) buf[i+4] = s[i]; // copy contents
    // set counter
    buf[0] = (buffer_counter & 0xff00 ) >> 8;
    buf[1] = buffer_counter & 0xff;
    buf[2] = l;  // set length
    buf[3] = request_bus; // if given, request timeout for local master
    request_bus=0; // reset again
    buffer =  buf; // swap to updated buffer
    inwrite = false; // leave critical region
  }
  interrupts();
}

# define ULNOIOT_I2C_END_DELAY 10
void UlnoiotI2c::suspend( int timems ) {
  timems += ULNOIOT_I2C_END_DELAY;
  // scale down time
  unsigned int t=0;
  if( timems<0 ) timems=0;
  else if( timems>5000 ) timems = 5000; // max 5s -> TODO: move to constants
  if( timems>=100 ) {
    t+=128; // set high bit
    timems = (timems+50) / 100;
  }
  t += timems;
  // Wait/block until new request has been sent
  noInterrupts();
  request_confirmed = false;
  request_bus = t;
  interrupts();
  // block until request came in but max maxblock
  int counter = 0;
  // Arduinos compiler just doesn't allow to read the request confirmed
  bool request_copy;
  while (true) {
    noInterrupts(); // i finterrupt clause not given, it is just cahced and never read
    request_copy = request_confirmed;
    interrupts();
    if (request_copy) break;
    delayMicroseconds(1000);
    counter += 1;
    if (counter >= ULNOIOT_I2C_REQUEST_MAXBLOCK ) {
      Serial.println("ulnoiot i2c maxblock reached, continuing");
      break;
    }
  }
  delayMicroseconds(1000*ULNOIOT_I2C_END_DELAY);
  /*Serial.print("Request confirmed: ");
  Serial.print(request_confirmed);
  Serial.print(" counter: ");
  Serial.println(counter);  // debug */
}

void UlnoiotI2c::receive(int count) {
  receive_buffer_size = 0;
  while(Wire.available()) { // loop through all
    if( receive_buffer_size >= ULNOIOT_I2C_BUFFER_SIZE ) break;
    receive_buffer[receive_buffer_size] = Wire.read();
    receive_buffer_size ++;
  }
  receive_buffer[receive_buffer_size] = 0; // terminate string
  if(receive_callback)
    receive_callback( receive_buffer, receive_buffer_size );
  // Serial.println(uvoid UlnoiotI2c::receive(int count)lnoiot_i2c_receive_buffer); // debug
}

UlnoiotI2c *_ulnoiot_i2c_last_init; // break out of objects into static environment
void _ulnoiot_request_caller() {
    _ulnoiot_i2c_last_init->request();
}
void _ulnoiot_receive_caller(int count) {
    _ulnoiot_i2c_last_init->receive(count);
}

void UlnoiotI2c::init(int init_time) {
  Wire.begin(ULNOIOT_I2C_ADDRESS); // join i2c bus with respective address
  _ulnoiot_i2c_last_init = this;
  Wire.onRequest(_ulnoiot_request_caller); // register request function (to send data to master)
  Wire.onReceive(_ulnoiot_receive_caller); // register receive function (to receive data from master)
  write(""); // init empty buffer
  delayMicroseconds(3000000); // let things settle TODO: check if necessary this long
  suspend(init_time); // wait given time
}

/*void ulnoiot_i2c_reinit() {
  Wire.onRequest(ulnoiot_i2c_request); // register request event (to send data to master)
  Wire.onReceive(ulnoiot_i2c_receive); // register receive event (to receive data from master)
}

void ulnoiot_i2c_deinit() {
  Wire.onRequest(NULL); // de-register request event
  Wire.onReceive(NULL); // de-register receive event
}*/
