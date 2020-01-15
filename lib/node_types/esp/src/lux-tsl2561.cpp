// lux-tsl2561.cpp
#include "lux-tsl2561.h"

Lux_TSL2561::Lux_TSL2561(const char* name, bool gain) :
    I2C_Device(name) {
    
    add_subdevice(new Subdevice("")); // 0
    _gain = gain;
    set_address(0x29);
}


void static printError(byte error)
  // If there's an I2C error, this function will
  // print out an explanation.
{
  Serial.print(F("I2C error: "));
  Serial.print(error,DEC);
  Serial.print(F(", "));
  
  switch(error)
  {
    case 0:
      Serial.println(F("success"));
      break;
    case 1:
      Serial.println(F("data too long for transmit buffer"));
      break;
    case 2:
      Serial.println(F("received NACK on address (disconnected?)"));
      break;
    case 3:
      Serial.println(F("received NACK on data"));
      break;
    case 4:
      Serial.println(F("other error"));
      break;
    default:
      Serial.println(F("unknown error"));
  }
}


void Lux_TSL2561::i2c_start() {
    sensor = new SFE_TSL2561();

    if(sensor) {
        unsigned char ID;

        sensor->begin(); // TODO: timeout

        if (sensor->getID(ID))
        {
            Serial.print(F("Got factory ID: 0X"));
            Serial.print(ID,HEX);
            Serial.println(F(", should be 0X5X"));
            sensor->setTiming(_gain, 3);
            // To start taking measurements, power up the sensor:
            sensor->setPowerUp();
            sensor->manualStart();
            last_read = millis();
            _started = true;
        }
        // Most library commands will return true if communications was successful,
        // and false if there was a problem. You can ignore this returned value,
        // or check whether a command worked correctly and retrieve an error code:
        else
        {
            byte error = sensor->getError();
            printError(error);
        }
    } else {
        ulog(F("Can't reserve memory for TSL2561."));
    }
}

#define INTEGRATION 250

bool Lux_TSL2561::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < INTEGRATION) return false;
    last_read = current_time; 

    sensor->manualStop();

    unsigned int data0, data1;
  
    if (sensor->getData(data0,data1))
    {
  
        // To calculate lux, pass all your settings and readings
        // to the getLux() function.
        
        // The getLux() function will return 1 if the calculation
        // was successful, or 0 if one or both of the sensors was
        // saturated (too much light). If this happens, you can
        // reduce the integration time and/or gain.
        // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor
  
        double lux;    // Resulting lux value
        boolean good;  // True if neither sensor is saturated
        
        // Perform lux calculation:

        good = sensor->getLux(_gain,INTEGRATION,data0,data1,lux);
        
        // Print out the results:
        
        if (discardval>0) {
            discardval --;
        } else {
            value().from((int)round(lux)); // lx
        }
        //if (good) Serial.println(F(" (good)"); else Serial.println(" (BAD)"));
    }
    else
    {
        // getData() returned false because of an I2C error, inform the user.

        byte error = sensor->getError();
        printError(error);
    }

    last_read = millis();
    sensor->manualStart();

    return true;
}
