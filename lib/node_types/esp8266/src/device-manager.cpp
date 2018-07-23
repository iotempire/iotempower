// device-manager.cpp
// author: ulno
// created: 2018-07-16

#include <map>
#include <iterator>

#include <PubSubClient.h>
#include <toolbox.h>
#include <device-manager.h>

static Device* device_list = new std::map<char*,Device>;
static int device_count = 0;

static int find_device(const char* name) {
    Ustring uname(name);
    for(int i=0; i<device_count; i++) {
        if(uname.equals(device_list[i]->get_name()))
            return i;
    }
    return -1;
}

bool add_device(Device &device) {
    if(device_count < ULNOIOT_MAX_DEVICES) {
        device_list[device_count] = &device;
        device_count ++;
        return true;
    } 
    return false;
}

// out of simplicity reasons, we don't allow the deletion of devices
// as we dropped interactive configuration in the transition from
// micropython to C
//
// bool remove_device(const char* name) {
//     int dev_nr = find_device(name);
//     if(dev_nr>=0) {
//         for(int i=dev_nr; i<device_count-1; i++) {
//             device_list[i] = device_list[i+1];
//         }
//         // attention, no dealloc/free happening here -> task of caller
//         return true;
//     }
//     return false;
// }

bool devices_update() {
    bool updated = false;
    Device *dev;
    for( int devnr=0; devnr<device_count; devnr++) {
        dev = device_list[devnr];
        updated |= dev->update();
    }
    return updated;
}


static unsigned long last_time = micros();

bool devices_publish(PubSubClient& mqtt_client, Ustring& node_topic) {
    // global: _last_publish
    unsigned long current_time = micros();
    if(current_time - last_time >= MIN_PUBLISH_TIME_US) {
        last_time = current_time;
        Serial.println("Publishing:");

        Ustring topic;

        Device *dev;
        for( int devnr=0; devnr<device_count; devnr++) {
            dev = device_list[devnr];
            const Ustring& val = dev->value();
            if(!val.empty()) {
                // construct full topic
                topic.from(node_topic);
                topic.add("/");
                topic.add(dev->get_name());
                Serial.print(dev->get_name().as_cstr());
                Serial.print(" ");
                Serial.println(val.as_cstr());
                if(!mqtt_client.publish(topic.as_cstr(), val.as_cstr())) {
                    // TODO: signal error and trigger reconnect
                    return false;
                }
            }
        } // end for - iterate over devices
    } // end if - time for new publish passed
    return true;
}

bool devices_subscribe(PubSubClient& mqtt_client, Ustring& node_topic) {
    // subscribe to all devices that accept input

    Ustring topic;

    Device *dev;
    for( int devnr=0; devnr<device_count; devnr++) {
        dev = device_list[devnr];

        // construct full topic
        topic.from(node_topic);
        topic.add("/");
        topic.add(dev->get_name());
        Serial.print("Subscribing to: ");
        Serial.println(topic.as_cstr());
        mqtt_client.subscribe(topic.as_cstr());
    } // end for - iterate over devices
}

// TODO: timestack, do_later

