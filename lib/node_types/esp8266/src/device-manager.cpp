// device-manager.cpp
// author: ulno
// created: 2018-07-16

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <toolbox.h>
#include <device-manager.h>

static Fixed_Map<Device, ULNOIOT_MAX_DEVICES> device_list;

/*static Device* find_device(const Ustring& name) {
    return device_list.find(name);
}

static Device* find_device(const char* name) {
    return device_list.find(name);
}*/

bool add_device(Device &device) {
    return device_list.add(&device);
}

// out of simplicity reasons, we don't allow the deletion of devices
// as we dropped interactive configuration in the transition from
// micropython to C

bool devices_update() {
    bool updated = false;
    device_list.for_each( [&] (Device& dev) {
        updated |= dev.update();
        return true;
    } );
    return updated;
}


static unsigned long last_time = micros();

bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    // global: last_time
    unsigned long current_time = micros();
    if(current_time - last_time >= MIN_PUBLISH_TIME_US) {
        last_time = current_time;
        Serial.println("Publishing:");

        Ustring topic;

        device_list.for_each( [&] (Device& dev) {
            dev.subdevices_for_each( [&] (Subdevice& sd) {
                const Ustring& val = sd.value();
                if(!val.empty()) {
                    // construct full topic
                    topic.from(node_topic);
                    topic.add("/");
                    topic.add(dev.get_name());
                    const Ustring& sd_name = sd.get_name();
                    if(!sd_name.empty()) { // only add, if name given
                        topic.add("/");
                        topic.add(sd.get_name());
                    }
                    Serial.print(topic.as_cstr()+node_topic.length()+1);
                    Serial.print(" ");
                    Serial.println(val.as_cstr());

                    if(!mqtt_client.publish(topic.as_cstr(), 0, true, val.as_cstr())) {
                        // TODO: signal error and trigger reconnect
                        return false;
                    }
                }
                return true; // continue loop
            } ); // end for_each - iterate over subdevices
            return true; // continue loop
        } ); // end for_each - iterate over devices
    } // end if - time for new publish passed
    return true;
}


bool devices_subscribe(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    // subscribe to all devices that accept input
    Ustring topic;

    device_list.for_each( [&] (Device& dev) {
        dev.subdevices_for_each( [&] (Subdevice& sd) {
            if(sd.subscribed()) {
                // construct full topic
                topic.from(node_topic);
                topic.add("/");
                topic.add(dev.get_name());
                topic.add("/");
                topic.add(sd.get_name());
                Serial.print("Subscribing to: ");
                Serial.println(topic.as_cstr());
                uint16_t packetIdSub = mqtt_client.subscribe(topic.as_cstr(), 0);
                Serial.print("Subscribing at QoS 0, packetId: ");
                Serial.println(packetIdSub);
            }
            return true; // continue subdevice loop
        } ); // end foreach - iterate over subdevices
        return true; //continue loop device loop
    } ); // end foreach - iterate over devices
    return true; // TODO: decide if error occured
}

bool devices_receive(Ustring& subtopic, Ustring& payload) {
    Ustring topic;

    // find matching subdevice for this topic/payload
    // subtopic is without node_topic
    device_list.for_each( [&] (Device& dev) {
        dev.subdevices_for_each( [&] (Subdevice& sd) {
            if(sd.subscribed()) {
                // construct full topic
                topic.from(dev.get_name());
                topic.add("/");
                topic.add(sd.get_name());
                Serial.print("Trying to match ");
                Serial.print(subtopic.as_cstr());
                Serial.print(" with ");
                Serial.println(topic.as_cstr());
                if(topic.equals(subtopic)) { // found match
                    payload.ignore_case(dev.get_ignore_case());
                    sd.call_receive_cb(payload);
                }
            }
            return true; //continue subdevice loop
        } ); // end foreach - iterate over subdevices
        return true; //continue loop device loop
    } ); // end foreach - iterate over devices

    return true;
}

// TODO: timestack, do_later

