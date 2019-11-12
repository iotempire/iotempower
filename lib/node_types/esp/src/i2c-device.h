// i2c-device.h
// author: ulno
// created: 2018-09-11
//
// adding a way to support multiple i2c busses

#ifndef _IOTEMPOWER_I2C_DEVICE_H_
#define _IOTEMPOWER_I2C_DEVICE_H_

//// prevent Wire instance to be allocated // too late, let's better overwrite
//#define NO_GLOBAL_TWOWIRE

#include <Arduino.h>
#include <Wire.h>
#include <device.h>

// Sometimes, we need master address to receive messages back
#define IOTEMPOWER_DEFAULT_I2C_MASTER_ADDRESS 8

#define IOTEMPOWER_DEFAULT_I2C_CLOCK 400000

#define IOTEMPOWER_DEFAULT_I2C_POLLRATE 100000

// seems like esp8266 has only sw i2c
// compare here: https://bbs.espressif.com/viewtopic.php?t=1032

class I2C_Device : public Device {
    private:
        // own wire: TODO: only reserve once per I2C-bus
    #ifdef ESP32
        TwoWire mywire = TwoWire(0); // ESP32 has two busses TODO: see if iotempower emulation works or needs to be distributed 
    #else
        TwoWire mywire = Wire;
    #endif
        uint8_t sda_pin;
        uint8_t scl_pin;
        uint8_t _i2c_address = 255; // This is the client device adddress
        uint8_t _master_address = IOTEMPOWER_DEFAULT_I2C_MASTER_ADDRESS;
        unsigned int clock_speed;
        void init_i2c(uint8_t sda, uint8_t scl, unsigned int clock,
                uint8_t master_address) {
            sda_pin = sda;
            scl_pin = scl;
            _master_address = master_address;
            clock_speed = clock;
        }
        bool run_scan_at_start = true;
        /* start is now private, check i2c_start for overwrite
         * TODO: check, why overwrite still works */
        void start();
    public:
        I2C_Device(const char* name, uint8_t client_address) : Device(name, IOTEMPOWER_DEFAULT_I2C_POLLRATE) {
            init_i2c(SDA, SCL, IOTEMPOWER_DEFAULT_I2C_CLOCK,
                IOTEMPOWER_DEFAULT_I2C_MASTER_ADDRESS);
            set_address(client_address);
        }

        I2C_Device(const char* name) : Device(name, IOTEMPOWER_DEFAULT_I2C_POLLRATE) {
            init_i2c(SDA, SCL, IOTEMPOWER_DEFAULT_I2C_CLOCK, 
                IOTEMPOWER_DEFAULT_I2C_MASTER_ADDRESS);
        }

        I2C_Device& i2c(uint8_t sda, uint8_t scl, unsigned int clock = 0,
                unsigned int master_address = 255) {
            if(clock==0) clock = clock_speed;
            if(master_address == 255) master_address = _master_address;
            init_i2c(sda, scl, clock, master_address);
            return *this;
        }
        I2C_Device& i2c(unsigned int clock) {
            init_i2c(sda_pin, scl_pin, clock, _master_address);
            return *this;
        }

        I2C_Device& set_address(uint8_t client_address) {
            _i2c_address = client_address;
            return *this;
        }
        I2C_Device& with_address(uint8_t client_address) {
            return with_address(client_address);
        }
        I2C_Device& address(uint8_t client_address) {
            return with_address(client_address);
        }

        I2C_Device& set_master(uint8_t address) {
            _master_address = address;
            return *this;
        }
        I2C_Device& with_master(uint8_t address) {
            return set_master(address);
        }
        I2C_Device& master(uint8_t address) {
            return set_master(address);
        }
        
        I2C_Device& disable_scan() {
            run_scan_at_start = false;
            return *this;
        }

        uint8_t get_sda() { return sda_pin; }
        uint8_t get_scl() { return scl_pin; }
        uint8_t get_address() { return _i2c_address; }
        uint8_t get_master() { return _master_address; }
        unsigned int get_clock() { return clock_speed; }
        TwoWire& get_wire() { return mywire; }

        /**
         * Check if device connected (returns true if it is found);
         * This should be called usually in start
         * after bus is initialized.
         * If called outside needs to be encapsuled
         * by measure_init and measure_exit.
         */
        bool scan();

        /**
         * This routine turns off the respective I2C bus and clears it
         * on return SCA and SCL pins are tri-state inputs.
         * You need to call Wire.begin() after this to re-enable I2C
         * This routine does NOT use the Wire library at all.
         *
         * returns 0 if bus cleared
         *         1 if SCL held low.
         *         2 if SDA held low by slave clock stretch for > 2sec
         *         3 if SDA held low after 20 clocks.
         * Check cpp code for copyright notice.
         */
        int clear_bus();

        /* measure_init
         * all i2c params have to be reinitialized all the time.
         * */
        void measure_init(); 

        /* measure_init
         * all i2c params have to be restored all the time.
         * */
        void measure_exit(); 

        // i2c devices need to overwrite i2c_start instead of start
        virtual void i2c_start() { _started = true; }


        // keep this as in device
        /* measure
         * TODO: make protected?
         * */
        virtual bool measure() { return true; }
};

#endif // _IOTEMPOWER_I2C_DEVICE_H_
