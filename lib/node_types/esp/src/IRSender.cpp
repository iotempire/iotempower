// IRSender.cpp
#include "IRSender.h"

IRSender::IRSender(const char* name, long nec, long signalPort) :
    Device(name, 100000) {
        _nec = nec;
        _signalPort = signalPort;
    add_subdevice(new Subdevice(F("")));
}
void IRSender::start() {
    sensor = new IRsend(_signalPort);

    if(sensor) {
        sensor -> begin();
        _started = true;
    } else {
        ulog(F("Can't reserve memory for the IRSender."));
    }
}


bool IRSender::measure() {
    value().clear();
    sensor->sendNEC(_nec);
    value().from(_nec);
    delay(2000);
    return false;
}