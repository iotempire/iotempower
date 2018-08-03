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


/* measure and filter
 */
bool devices_measure() {
    bool measured=false;
    device_list.for_each( [&] (Device& dev) {
        if(dev.poll_measure()) {
            dev.check_changes();
        }
        return true; // Continue loop
    } );
    return measured;
}

/* check if changes have to be published
 * -> eventually publish
 * This is only called with enough time distance so send buffer doesn't run over.
 * Careful a value causing a change could already have been overwritten.
 * Return true if anything has been published.
 * Return false if nothing was published.
 */
bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all) {
    bool published = false;
    device_list.for_each( [&] (Device& dev) {
        if(publish_all || dev.needs_publishing()) {
            dev.publish(mqtt_client, node_topic);
            published = true;
        }
        return true; // Continue loop
    } );
    return published;
}


bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    return devices_publish(mqtt_client, node_topic, false); 
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
                // Serial.print("Trying to match ");
                // Serial.print(subtopic.as_cstr());
                // Serial.print(" with ");
                // Serial.println(topic.as_cstr());
                if(topic.equals(subtopic)) { // found match
                    payload.ignore_case(dev.get_ignore_case());
                    sd.call_receive_cb(payload);
                    return false; // found no further search necessary
                }
            }
            return true; //continue subdevice loop
        } ); // end foreach - iterate over subdevices
        return true; //continue loop device loop
    } ); // end foreach - iterate over devices

    return true;
}

// TODO: timestack, do_later

