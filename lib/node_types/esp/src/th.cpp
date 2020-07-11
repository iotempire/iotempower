// th.cpp
#include "th.h"

bool Dht11::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time; 
    value(0).from(_dht->getTemperature());
    value(1).from(_dht->getHumidity());
    return true;
}

// TODO: merge with Dht11 as very similar
bool Dht22::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time; 
    value(0).from(_dht->getTemperature());
    value(1).from(_dht->getHumidity());
    return true;
}

bool Ds18b20::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time;
    sensors->requestTemperatures();
    float t = sensors->getTempC(first);
    value().from(t);
    return true;
}