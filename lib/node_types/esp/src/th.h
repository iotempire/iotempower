// analog.h
// Header File for digital temperature and humidity devices

#ifndef _TH_H_
#define _TH_H_

#include <Arduino.h>
#include <device.h>

#include <DHTesp.h>

// does not work with autodetect anymore
class Dht11 : public Device {
    private:
        DHTesp *_dht = NULL;
        uint8_t _pin;
        unsigned long _read_interval = 2000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
    public:
        Dht11(const char* name, uint8_t pin) :
            Device(name, 10000) {
            _dht = new DHTesp();
            if(_dht) {
                ulog(F("DHT11 initialized."));
                _pin = pin;
                add_subdevice(new Subdevice(F("temperature")));
                add_subdevice(new Subdevice(F("humidity")));
            } else {
                ulog(F("Trouble reserving memory for DHT11."));
            }
        }
        void start() {
            if(_dht) {
                _dht->setup(_pin, DHTesp::DHT11);
                _started = true;
                measure();
            }
        }
        bool measure();
};

// TODO: merge similar code into super class
class Dht22 : public Device {
    private:
        DHTesp *_dht = NULL;
        uint8_t _pin;
        unsigned long _read_interval = 2000; // only read every interval ms
        unsigned long last_read = millis() - _read_interval;
    public:
        Dht22(const char* name, uint8_t pin) :
            Device(name, 10000) {
            _dht = new DHTesp();
            if(_dht) {
                ulog(F("DHT22 initialized."));
                _pin = pin;
                add_subdevice(new Subdevice(F("temperature")));
                add_subdevice(new Subdevice(F("humidity")));
            } else {
                ulog(F("Trouble reserving memory for DHT22."));
            }
        }
        void start() {
            if(_dht) {
                _dht->setup(_pin, DHTesp::DHT22);
                _started = true;
                measure();
            }
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
            Device(name, 10000) {
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
                ulog(F("Unable to find address for Device 0. Sensor will not work."));
                return;
            } 
            _started = true;
            measure();
        }
        bool measure();
        ~Ds18b20() {}
};


#endif // _ANALOG_H_