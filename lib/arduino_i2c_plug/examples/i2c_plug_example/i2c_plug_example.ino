// Example for an i2c connector plug to communicate with iotempower-node via i2c
//
// Author: ulno (http://ulno.net)
// Created: 2017-10-20
//
// In an IoTempower node setup, a corresponding socket would look like this:
// i2c_socket(arduino1).pollrate(2000);
//

#include <i2c-plug.h>

void myreceive( char *msg, int len ) {
    Serial.print("Received a message of length: ");
    Serial.println(len);
    Serial.print("The message is:");
    Serial.println(msg); // is properly 0 terminated - but len can be used to
}


void setup() {
    Serial.begin(115200); // start serial for debug output
    delay(500); // Let serial settle, so we can see
    Serial.println("Serial is setup. Program starting.");
    I2C_Plug.begin(9, myreceive);
}


void loop() {
    static int counter = 0;
    String s;
    
    s=String(counter);
    Serial.print("Sending: ");
    Serial.println(s);
    I2C_Plug.write(s);
    delay(1000);
    counter += 1;
}


