// Example for an i2c connector to communicate with ulnoiot-node via i2c
//
// Based on Wire example by Nicholas Zambetti <http://www.zambetti.com>

// Author: ulno (http://ulno.net)
// Created: 2017-10-20

#define I2C_ADDRESS 8

// real buffer takes first two bytes as counter and next as size
#define I2C_BUFFER_SIZE 64

#include <Wire.h>

char ulnoiot_i2c_buffer1[I2C_BUFFER_SIZE];
char ulnoiot_i2c_buffer2[I2C_BUFFER_SIZE];
char *ulnoiot_i2c_buffer = ulnoiot_i2c_buffer1;
char ulnoiot_i2c_receive_buffer[I2C_BUFFER_SIZE+1]; // one more which can be 0 for end
int ulnoiot_i2c_receive_buffer_size = 0;

int ulnoiot_i2c_buffer_counter = 0;

typedef void (*ulnoiot_i2c_receive_callback_type)(char *, int);
ulnoiot_i2c_receive_callback_type ulnoiot_i2c_receive_callback = NULL;

bool ulnoiot_i2c_inwrite=false; // ulnoiot is in write critical region
bool ulnoiot_i2c_inrequest=false; // ulnoiot is in request

void ulnoiot_i2c_request() {
  char *buf = ulnoiot_i2c_buffer; // pick one buffer
  ulnoiot_i2c_inrequest=true;
  Wire.write(buf, ((unsigned int)buf[2])+3); // respond
  ulnoiot_i2c_inrequest=false;
}

void ulnoiot_i2c_write(String s) {
  char *buf;

  // problem is that this function could interrupt the request
  // therefore we use double buffering.
  noInterrupts(); // this needs to be written atomically
  if (!ulnoiot_i2c_inwrite) { // enter only once
    ulnoiot_i2c_inwrite=true; // enter critical region
    
    if( ulnoiot_i2c_buffer == ulnoiot_i2c_buffer1 ) { // write to inactive buffer
      buf = ulnoiot_i2c_buffer2;
    } else {
      buf = ulnoiot_i2c_buffer1;
    }
    int l = s.length();
    if(l > I2C_BUFFER_SIZE - 3) { // ignore if string too long
      l = I2C_BUFFER_SIZE - 3;
    }
    ulnoiot_i2c_buffer_counter += 1;
    for( int i=0; i<l; i++ ) buf[i+3] = s[i]; // copy contents
    buf[2] = l;  // set length
    // set counter
    buf[0] = (ulnoiot_i2c_buffer_counter & 0xff00 ) >> 8;
    buf[1] = ulnoiot_i2c_buffer_counter & 0xff;
    ulnoiot_i2c_buffer =  buf; // swap to updated buffer
    ulnoiot_i2c_inwrite = false; // leave critical region
  }
  interrupts();
}

void ulnoiot_i2c_receive(int count) {
  ulnoiot_i2c_receive_buffer_size = 0;
  while(Wire.available()) { // loop through all
    if( ulnoiot_i2c_receive_buffer_size >= I2C_BUFFER_SIZE ) break;
    ulnoiot_i2c_receive_buffer[ulnoiot_i2c_receive_buffer_size] = Wire.read();
    ulnoiot_i2c_receive_buffer_size ++;
  }
  ulnoiot_i2c_receive_buffer[ulnoiot_i2c_receive_buffer_size] = 0; // terminate string
  if(ulnoiot_i2c_receive_callback)
    ulnoiot_i2c_receive_callback( ulnoiot_i2c_receive_buffer, ulnoiot_i2c_receive_buffer_size );
  // Serial.println(ulnoiot_i2c_receive_buffer); // debug
}

void ulnoiot_i2c_init() {
  Wire.begin(I2C_ADDRESS); // join i2c bus with respective address
  Wire.onRequest(ulnoiot_i2c_request); // register request event (to send data to master)
  Wire.onReceive(ulnoiot_i2c_receive); // register receive event (to receive data from master)
  ulnoiot_i2c_write(""); // init empty buffer
}

void ulnoiot_i2c_reinit() {
  Wire.onRequest(ulnoiot_i2c_request); // register request event (to send data to master)
  Wire.onReceive(ulnoiot_i2c_receive); // register receive event (to receive data from master)
}

void ulnoiot_i2c_deinit() {
  Wire.onRequest(NULL); // de-register request event
  Wire.onReceive(NULL); // de-register receive event
}


///////// example main program, replace with your own
void myreceive( char *msg, int len ) {
    Serial.print("Received a message of length: ");
    Serial.println(len);
    Serial.print("Message:");
    Serial.println(msg); // is properly 0 terminated - but len can used to
}

void setup() {
  ulnoiot_i2c_receive_callback = myreceive; // call before init
  ulnoiot_i2c_init(); // call this to start the ulnoiot connector (call last in setup)
  Serial.begin(115200); // start serial for debug output
}

int counter = 0;

void loop() {
  char mystr[20];
  ulnoiot_i2c_deinit();
  snprintf(mystr, 19, "%d", counter);
  ulnoiot_i2c_write(mystr);
  Serial.println(mystr);
  ulnoiot_i2c_reinit();
  delay(1000);
  counter += 1;
}


