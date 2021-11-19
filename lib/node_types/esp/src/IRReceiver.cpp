// IRReceiver.cpp
#include "IRReceiver.h"

IRReceiver::IRReceiver(const char* name, long signalPort) :
    Device(name, 100000) {
        _signalPort = signalPort;
    add_subdevice(new Subdevice(F("")));
}
void IRReceiver::start() {
    sensor = new IRrecv(_signalPort);
    if(sensor) {
        sensor -> enableIRIn();
        _started = true;
    } else {
        ulog(F("Can't reserve memory for IRReceiver."));
    }
}


bool IRReceiver::measure() {
    decode_results results;
    value().clear();

    if(sensor->decode(&results)){
        value().printf("%lx", results.value);
        sensor -> resume();
        return true;
    }
    
    return false;
}