// analog.h
// Header File for digital temperature and humidity devices

#ifndef _TH_H_
#define _TH_H_

#include <Arduino.h>
#include <device.h>

#include <DHT.h>

class Dht11 : public Device {
    private:
        DHT _dht;
        uint8_t _pin;
        unsigned long _read_interval = 2000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
    public:
        Dht11(const char* name, uint8_t pin) :
            Device(name) {
            _pin = pin;
            _dht.setup(_pin);
            add_subdevice(new Subdevice("temperature"));
            add_subdevice(new Subdevice("humidity"));
            measure();
        }
        bool measure();
};

#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20 : public Device {
    private:
        OneWire *one_wire;
        uint8_t _pin;
        DeviceAddress first;
        DallasTemperature *sensors;
        unsigned long _read_interval = 1000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
        bool initialized = false;
    public:
        Ds18b20(const char* name, uint8_t pin) :
            Device(name) {
            _pin = pin;
            one_wire = new OneWire(_pin); // TODO: think about a destructor
            sensors = new DallasTemperature(one_wire);
            add_subdevice(new Subdevice("")); // TODO: think about a destructor
            sensors->begin();
            if (!sensors->getAddress(first, 0)) {
                log("Unable to find address for Device 0. Sensor will not work.");
            } else {
                initialized = true;
            }
            measure();
        }
        bool is_initialized() { return initialized; }
        bool measure();
        ~Ds18b20() {

        }
};


#endif // _ANALOG_H_