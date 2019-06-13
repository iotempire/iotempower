// gesture-apds9960.cpp
#include "gesture-apds9960.h"

Gesture_Apds9960::Gesture_Apds9960(const char* name,
            bool light, bool proximity, bool gesture) :
    I2C_Device(name) {
    
    int8_t subd_nr=0;
    if(light) {
        add_subdevice(new Subdevice("color")); // 0
        _light = subd_nr;
        subd_nr ++;
    }
    if(proximity) {
        add_subdevice(new Subdevice("proximity")); // 1
        _proximity = subd_nr;
        subd_nr ++;
    }
    if(gesture) {
        add_subdevice(new Subdevice("gesture")); // 2
        gesture = subd_nr;
        subd_nr ++;
    }
    set_address(APDS9960_I2C_ADDR); // address will always be this one (hardcoded in library)
    pollrate(500); // need to be not too fast
}


void Gesture_Apds9960::i2c_start() {
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

        // Start light sensor
        if ( sensor->enableLightSensor(false) ) {
            ulog("Light sensor is now running");
        } else {
            ulog("Something went wrong during light sensor init!");
            return;
        }

        // Proximity sensor
        
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

        // init gesture (after proximity?)
        if (sensor->enableGestureSensor(false)) {
            //sensor->setLEDBoost(LED_BOOST_100); // comment from https://github.com/jonn26/SparkFun_APDS-9960_Sensor_Arduino_Library
            ulog("Gesture sensor is now running");
        } else {
            ulog("Something went wrong during gesture sensor init!");
            return;
        }

        // use pollrate instead last_read = millis();
        _started = true;        
    } else {
        ulog("Can't reserve memory for APDS9960.");
    }
}

bool Gesture_Apds9960::measure() {
    // done now by pollrate
    // unsigned long current_time = millis();
    // if(current_time - last_read < 250) return false;
    // last_read = current_time; 

    // make sure the sensor has a 100uF capacitor from gnd to vcc
    // (short leg ground, long leg vcc)
    // TODO: check if this works at once
    // expose more tuning values

    uint16_t ambient_light = 0;
    uint16_t red_light = 0;
    uint16_t green_light = 0;
    uint16_t blue_light = 0;


    if(_light >= 0) {
        if (  !sensor->readAmbientLight(ambient_light) ||
                !sensor->readRedLight(red_light) ||
                !sensor->readGreenLight(green_light) ||
                !sensor->readBlueLight(blue_light) ) {
            ulog("Error reading light values");
        } else { // success
            measured_value(_light).printf("%d,%d,%d,%d", 
                ambient_light, red_light, green_light, blue_light);
        }
    }

    if(_proximity >= 0) {
        uint8_t proximity_data = 0;

        if ( !sensor->readProximity(proximity_data) ) {
            ulog("Error reading proximity value");
        } else {
            if(_threshold == 0) {
                measured_value(_proximity).from((unsigned int)proximity_data);
            } else {
                if(proximity_data <= _threshold)
                    measured_value(_proximity).from(_high);
                else
                    measured_value(_proximity).from(_low);
            }
        }
    }

    if(_gesture >= 0) {
        if ( sensor->isGestureAvailable() ) {
            switch ( sensor->readGesture() ) {
            case DIR_UP:
                measured_value(_gesture).from("up");
                break;
            case DIR_DOWN:
                measured_value(_gesture).from("down");
                break;
            case DIR_LEFT:
                measured_value(_gesture).from("left");
                break;
            case DIR_RIGHT:
                measured_value(_gesture).from("right");
                break;
            case DIR_NEAR:
                measured_value(_gesture).from("near");
                break;
            case DIR_FAR:
                measured_value(_gesture).from("far");
                break;
            default:
                measured_value(_gesture).from("none");
            }
        }
    }

    return true;
}
