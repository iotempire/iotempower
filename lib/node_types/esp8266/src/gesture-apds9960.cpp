// gesture-apds9960.cpp
#include "gesture-apds9960.h"

Gesture_Apds9960::Gesture_Apds9960(const char* name, uint8_t proximity_threshold) :
    I2C_Device(name) {
    
    _proximity_threshold = proximity_threshold;
    add_subdevice(new Subdevice("color")); // 0
    add_subdevice(new Subdevice("proximity")); // 1
    add_subdevice(new Subdevice("gesture")); // 2
    set_address(APDS9960_I2C_ADDR); // address will always be this one (hardcoded in library)
}


void Gesture_Apds9960::start() {
//    sensor = new SparkFun_APDS9960(get_address());
    sensor = new SparkFun_APDS9960(); // address is hardcoded in library 0x39

    if(sensor) {
        // Start running the APDS-9960 gesture sensor engine
        if(sensor->init()) {
            ulog("APDS-9960 initialization complete");
        } else {
            ulog("Something went wrong during APDS-9960 init!");
            return;
        }
        if (sensor->enableGestureSensor(false)) {
            ulog("Gesture sensor is now running");
        } else {
            ulog("Something went wrong during gesture sensor init!");
            return;
        }

        // Start running the APDS-9960 light sensor (no interrupts)
        if ( sensor->enableLightSensor(false) ) {
            ulog("Light sensor is now running");
        } else {
            ulog("Something went wrong during light sensor init!");
            return;
        }

        // Adjust the Proximity sensor gain
        if ( !sensor->setProximityGain(PGAIN_2X) ) {
            ulog("Something went wrong trying to set PGAIN");
        }
        
        // Start running the APDS-9960 proximity sensor (interrupts)
        if ( sensor->enableProximitySensor(false) ) {
            ulog("Proximity sensor is now running");
        } else {
            ulog("Something went wrong during sensor init!");
            return;
        }

        last_read = millis();
        _started = true;        
    } else {
        ulog("Can't reserve memory for APDS9960.");
    }
}

bool Gesture_Apds9960::measure() {
    unsigned long current_time = millis();
    if(current_time - last_read < 100) return false;
    last_read = current_time; 

    uint16_t ambient_light = 0;
    uint16_t red_light = 0;
    uint16_t green_light = 0;
    uint16_t blue_light = 0;

    if (  !sensor->readAmbientLight(ambient_light) ||
            !sensor->readRedLight(red_light) ||
            !sensor->readGreenLight(green_light) ||
            !sensor->readBlueLight(blue_light) ) {
        ulog("Error reading light values");
    } else { // success
        measured_value(0).printf("%d,%d,%d,%d", 
            ambient_light, red_light, green_light, blue_light);
    }

    uint8_t proximity_data = 0;

    if ( !sensor->readProximity(proximity_data) ) {
        ulog("Error reading proximity value");
    } else {
        if(proximity_data > _proximity_threshold)
            measured_value(1).from(1);
        else if(proximity_data < 1)
            measured_value(1).from(0);
    }
    
    if ( sensor->isGestureAvailable() ) {
        switch ( sensor->readGesture() ) {
        case DIR_UP:
            measured_value(2).from("up");
            break;
        case DIR_DOWN:
            measured_value(2).from("down");
            break;
        case DIR_LEFT:
            measured_value(2).from("left");
            break;
        case DIR_RIGHT:
            measured_value(2).from("right");
            break;
        case DIR_NEAR:
            measured_value(2).from("near");
            break;
        case DIR_FAR:
            measured_value(2).from("far");
            break;
        default:
            measured_value(2).from("none");
        }
    }

    return true;
}
