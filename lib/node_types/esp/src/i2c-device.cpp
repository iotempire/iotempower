// i2c_device.cpp
// author: ulno
// created: 2018-09-11

#include "i2c-device.h"

void I2C_Device::start() {
    if(_master_address == _i2c_address) {
        ulog(F("I2C Master address equals client address - %d. Not starting device."), _i2c_address);
        return;
    }
    clear_bus();
    measure_init();
    if(run_scan_at_start) {
        if(scan()) {
            i2c_start(); // call device specific overwritten start
        } else {
            ulog(F("I2C-device with address 0x%x is not on bus, not starting it."), 
                _i2c_address);
        }
    } else {
        i2c_start(); // call device specific overwritten start
    }
    measure_exit();
}

void I2C_Device::measure_init() {
    // if(!cleared) {
    //     if(clear_bus()>0)
    //         ulog(F("Could not clear i2c bus on sda %d and scl %d."),
    //             sda_pin, scl_pin);
    //     cleared = true; // We don't try again to prevent blocking
    // }
    // TODO: check - this seems to destroy the display
//    Wire.begin(sda_pin, scl_pin, _master_address); // TODO: add again, destroys some i2c devices (incl. display)
    Wire = mywire; // overwrite current Wire to allow multiple i2c busses
    // reset sda and scl as they are global in twi
    Wire.begin(sda_pin, scl_pin); // works, better than giving master
    // TODO: consider adding an option for not having pull-ups, so pull them down here
    Wire.setClock(clock_speed); // reset clock speed
}

void I2C_Device::measure_exit() {
    mywire = Wire; // remember state of Wire to allow multiple i2c busses
}

bool I2C_Device::scan() {
    Wire.beginTransmission(_i2c_address);
    return Wire.endTransmission() == 0;
}

// The following is taken from: http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html
// Matthew Ford 1st August 2017 (original 28th September 2014)
// (c) Forward Computing and Control Pty. Ltd. NSW Australia
// https://github.com/esp8266/Arduino/issues/1025#issuecomment-319199959
int I2C_Device::clear_bus() {
#if defined(TWCR) && defined(TWEN)
    TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the sda_pin and scl_pin pins directly
#endif

    // using pullup is allowed here as also in esp8266 core twi-library (could damage bus-devices?)
    pinMode(sda_pin, INPUT_PULLUP); // Make sda_pin (data) and scl_pin (clock) pins Inputs with pullup.
    pinMode(scl_pin, INPUT_PULLUP);

    delay(250); // assert master

    boolean SCL_LOW = (digitalRead(scl_pin) == LOW); // Check is scl_pin is Low.
    if (SCL_LOW) { //If it is held low Arduino cannot become the I2C master. 
        return 1; //I2C bus error. Could not clear scl_pin clock line held low
    }

    boolean SDA_LOW = (digitalRead(sda_pin) == LOW);  // vi. Check sda_pin input.
    int clockCount = 20; // > 2x9 clock

    while (SDA_LOW && (clockCount > 0)) { //  vii. If sda_pin is Low,
        clockCount--;
        // Note: I2C bus is open collector so do NOT drive scl_pin or sda_pin high.
        pinMode(scl_pin, INPUT); // release scl_pin pullup so that when made output it will be LOW
        pinMode(scl_pin, OUTPUT); // then clock scl_pin Low
        delayMicroseconds(10); //  for >5uS
        pinMode(scl_pin, INPUT); // release scl_pin LOW
        pinMode(scl_pin, INPUT_PULLUP); // turn on pullup resistors again
        // do not force high as slave may be holding it low for clock stretching.
        delayMicroseconds(10); //  for >5uS
        // The >5uS is so that even the slowest I2C devices are handled.
        SCL_LOW = (digitalRead(scl_pin) == LOW); // Check if scl_pin is Low.
        int counter = 20;
        while (SCL_LOW && (counter > 0)) {  //  loop waiting for scl_pin to become High only wait 2sec.
            counter--;
            delay(100);
            SCL_LOW = (digitalRead(scl_pin) == LOW);
        }
        if (SCL_LOW) { // still low after 2 sec error
            return 2; // I2C bus error. Could not clear. scl_pin clock line held low by slave clock stretch for >2sec
        }
        SDA_LOW = (digitalRead(sda_pin) == LOW); //   and check sda_pin input again and loop
    }
    if (SDA_LOW) { // still low
        return 3; // I2C bus error. Could not clear. sda_pin data line held low
    }

    // else pull sda_pin line low for Start or Repeated Start
    pinMode(sda_pin, INPUT); // remove pullup.
    pinMode(sda_pin, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
    // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
    /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
    delayMicroseconds(10); // wait >5uS
    pinMode(sda_pin, INPUT); // remove output low
    pinMode(sda_pin, INPUT_PULLUP); // and make sda_pin high i.e. send I2C STOP control.
    delayMicroseconds(10); // x. wait >5uS
    pinMode(sda_pin, INPUT); // and reset pins as tri-state inputs which is the default state on reset
    pinMode(scl_pin, INPUT);

    return 0; // all ok
}
