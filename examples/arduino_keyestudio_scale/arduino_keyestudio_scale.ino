// Example for an i2c connector to communicate with iotempower-node via i2c
// This example is optimized for the keyes-scale
//
// Based on Wire example by Nicholas Zambetti <http://www.zambetti.com>
//
// Author: ulno (http://ulno.net)
// Created: 2017-10-21
//
// TODO: adjust to i2csocket/plug

#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h> 
//#include <LiquidCrystal_PCF8574.h> // http://www.mathertel.de/Arduino/LiquidCrystal_PCF8574.aspx
#include <math.h>
#include <UlnoiotI2c.h>

HX711_ADC hx(9, 10);

bool taring = false;

void trigger_tare( char *msg, int len ) {
    Serial.print("Received a message of length: ");
    Serial.println(len);
    Serial.print("Message:");
    Serial.println(msg); // is properly 0 terminated - but len can be used to
    if(len==4 && !strncmp("tare",msg,4)) {
      Serial.println("Tare request received.");
      hx.tareNoDelay();
      taring = true;
    }
}

UlnoiotI2c ui2c(1000, trigger_tare); // Must be initialized first (before other i2c)
// doesn't work LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

long t=0;
void setup() {
  Serial.begin(115200);
  /*Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x27);
  int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LCD found.");
  } else {
    Serial.print("Error: ");
    Serial.print(error);
    Serial.println(": LCD not found.");
  }

  lcd.setBacklight(255);
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Wait, starting");
  lcd.setCursor(0, 1);
  lcd.print("up for 5s ..."); */
  Serial.println("Wait 5s to start up and stabilize ...");
  hx.setGain(128);
  hx.begin();
  long stabilisingtime = 5000; // tare preciscion can be improved by adding a few seconds of stabilising time
  hx.start(stabilisingtime);
  hx.setCalFactor(448.5); // user set calibration factor (float)
  Serial.println("Startup + tare is complete");
  /*ui2c.suspend(1000); // let the lcd print some initial stuff
  lcd.clear();
  lcd.print("Tare and start");
  lcd.setCursor(0, 1);
  lcd.print("up complete.");
  delay(500);
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print("UlnoIoT scale");*/
}

void loop() {
  char mystr[20];
  long newt;
  int w;
  hx.update();
  if(digitalRead(0) == LOW) {
    hx.tareNoDelay();
    taring = true;
  }
  //get smoothed value from data set + current calibration factor
  newt = millis();
  if (newt > t + 500) {
    t = newt; // TODO: check overflow
    //float v = hx.getCalFactor();
    w = round(hx.getData());
    snprintf(mystr, 19, "%d", w);
    ui2c.write(mystr);
    Serial.print("Weight in g: ");
    Serial.println(w);
/*    ui2c.suspend(300); // Suspend iotempower i2c master to act myself as master
    lcd.setCursor(1, 1);
    lcd.print(w);
    lcd.print(" g");*/
    //check if last tare operation is complete
    if (hx.getTareStatus() == true) {
      taring = false;
      Serial.println("Tare complete");
    }
    /*ui2c.suspend(50); // Suspend iotempower i2c master to use act myself as master
    if (taring) {
      lcd.print(" taring  ");        
    } else {
      lcd.print("         ");        
    } */
  }
  delay(1); // prevent busy waiting
} 
