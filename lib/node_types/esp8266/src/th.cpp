// th.cc
#include "th.h"

bool Dht11::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time; 
    measured_value(0).from(_dht.getTemperature());
    measured_value(1).from(_dht.getHumidity());
    return true;
}

bool Ds18b20::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < _read_interval) return false;
    last_read = current_time;
    sensors->requestTemperatures();
    float t = sensors->getTempC(first);
    measured_value().from(t);
    return true;
}