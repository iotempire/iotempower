// i2c_device.h
// author: ulno
// created: 2018-09-11
//
// adding a way to support multiple i2c busses

#ifndef _ULNOIOT_I2C_DEVICE_H_
#define _ULNOIOT_I2C_DEVICE_H_

#include <Wire.h>
#include <device.h>

#define ULNOIOT_DEFAULT_I2C_CLOCK 100000
// seems like esp8266 has only sw i2c
// compare here: https://bbs.espressif.com/viewtopic.php?t=1032

class I2C_Device : public Device {
    private:
        uint8_t sda_pin;
        uint8_t scl_pin;
        uint8_t _i2c_address = 255;
        unsigned int clock_speed;
        void init_i2c(uint8_t sda, uint8_t scl, unsigned int clock) {
            sda_pin = sda;
            scl_pin = scl;
            clock_speed = clock;
        }
    public:
        I2C_Device(const char* name, uint8_t address) : Device(name) {
            init_i2c(SDA, SCL, ULNOIOT_DEFAULT_I2C_CLOCK);
            set_address(address);
        }

        I2C_Device(const char* name) : Device(name) {
            init_i2c(SDA, SCL, ULNOIOT_DEFAULT_I2C_CLOCK);
        }

        I2C_Device& i2c(uint8_t sda, uint8_t scl) {
            init_i2c(sda, scl, ULNOIOT_DEFAULT_I2C_CLOCK);
            return *this;
        }
        I2C_Device& i2c(uint8_t sda, uint8_t scl, unsigned int clock) {
            init_i2c(sda, scl, clock);
            return *this;
        }
        I2C_Device& set_address(uint8_t address) {
            _i2c_address = address;
            return *this;
        }
        I2C_Device& with_address(uint8_t address) {
            return with_address(address);
        }

        uint8_t get_sda() { return sda_pin; }
        uint8_t get_scl() { return scl_pin; }
        uint8_t get_address() { return _i2c_address; }

        /* measure_init
         * all i2c params have to be reinitilized all the time.
         * */
        void measure_init(); 

        // keep this as in device
        /* start
         * TODO: make protected?
         * */
        virtual void start() { _started = true; }

        // keep this as in device
        /* measure
         * TODO: make protected?
         * */
        virtual bool measure() { return true; }
};

#endif // _ULNOIOT_I2C_DEVICE_H_
