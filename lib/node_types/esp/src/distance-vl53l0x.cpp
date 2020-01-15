// distance-vl53l0x.cpp
#include <VL53L0X.h>
#include "distance-vl53l0x.h"

Distance_Vl53l0x::Distance_Vl53l0x(const char* name, 
                    bool long_range, bool high_accuracy_slow) :
        I2C_Device(name) {
    _long_range = long_range;
    _high_accuracy_slow = high_accuracy_slow;
    add_subdevice(new Subdevice(""));
    set_address(0b0101001); // TODO: careful if address is changed.
}

void Distance_Vl53l0x::i2c_start() {
    sensor = new VL53L0X();

    if(sensor) {
        sensor->init();
        sensor->setTimeout(500); // TODO: should this be adjusted?

        if(_long_range) {  // lower the return signal rate limit (default is 0.25 MCPS)
            sensor->setSignalRateLimit(0.1);
            // increase laser pulse periods (defaults are 14 and 10 PCLKs)
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
            sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        }

        // This might only work with clean bus (had random crashes here before)
        if(_high_accuracy_slow) {
            // increase timing budget to 200 ms
            sensor->setMeasurementTimingBudget(200000);
        } else {
            // reduce timing budget to 20 ms (default is about 33 ms)
            sensor->setMeasurementTimingBudget(20000);
        }


        // Start continuous back-to-back mode (take readings as
        // fast as possible).  To use continuous timed mode
        // instead, provide a desired inter-measurement period in
        // ms (e.g. sensor.startContinuous(100)).
        sensor->startContinuous();


        _started = true;
        measure();
    } else {
        ulog(F("Can't reserve memory for vl53l0x."));
    }
}


bool Distance_Vl53l0x::measure() {
    uint16_t distance = sensor->readRangeContinuousMillimeters();
    if (sensor->timeoutOccurred()) { 
        ulog(F("VL53L0X TIMEOUT or invalid distance"));
        return false; 
    }
    if(distance > IOTEMPOWER_VL53L0X_MAXDIST) {
        distance = IOTEMPOWER_VL53L0X_MAXDIST;
    }
    value().from(distance);
    return true;
}
