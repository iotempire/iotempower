// analog.h
// Header File for digital temperature and humidity devices

#ifndef _TH_H_
#define _TH_H_

#include <Arduino.h>
#include <device.h>

#include <DHT.h>

// should work with autodetect
class Dht : public Device {
    private:
        DHT _dht;
        uint8_t _pin;
        unsigned long _read_interval = 2000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
    public:
        Dht(const char* name, uint8_t pin) :
            Device(name) {
            _pin = pin;
            add_subdevice(new Subdevice("temperature"));
            add_subdevice(new Subdevice("humidity"));
        }
        void start() {
            _dht.setup(_pin);
            _started = true;
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
    public:
        Ds18b20(const char* name, uint8_t pin) :
            Device(name) {
            _pin = pin;
            add_subdevice(new Subdevice(""));
        }
        void start() {
             // TODO: think about destructor(s)
            one_wire = new OneWire(_pin);
            if(!one_wire) return;
            sensors = new DallasTemperature(one_wire);
            if(!sensors) return;
            sensors->begin();
            if (!sensors->getAddress(first, 0)) {
                ulog("Unable to find address for Device 0. Sensor will not work.");
                return;
            } 
            _started = true;
            measure();
        }
        bool measure();
        ~Ds18b20() {}
};


#endif // _ANALOG_H_