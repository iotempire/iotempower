// device-manager.cpp
// author: ulno
// created: 2018-07-16

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <toolbox.h>
#include <device-manager.h>

////// structure and list for scheduler
typedef std::function<void()> Do_Later_Callback;

typedef struct {
    unsigned long time;
    int16_t id;
    Do_Later_Callback callback;
} do_later_map_t;

static do_later_map_t do_later_map[ULNOIOT_DO_LATER_MAP_SIZE];
static int do_later_map_count=0;

bool do_later_is_full() {
    return do_later_map_count >= ULNOIOT_DO_LATER_MAP_SIZE;
}

static void do_later_delete(int pos) {
    if(do_later_map_count<=0) return;
    pos = limit(pos, 0, do_later_map_count-1);
//    ulog("do_later_delete pos: %d  count: %d",pos,do_later_map_count);
    memmove(do_later_map+pos, do_later_map+pos+1, sizeof(do_later_map_t)*(do_later_map_count-pos));
    do_later_map_count --;
}

static bool do_later_add(unsigned long in_ms, int16_t id, Do_Later_Callback cbu) {
    bool forgot_one = false;
    // insert sorted into list
    unsigned long current = millis();
    in_ms += current;
    int pos; // position variable for finding things
    // if valid id (!=-1) is given, a potential existing position needs 
    // to be deleted
    if(id!=-1) {
        // check if already in there
        for(pos=0; pos<do_later_map_count; pos++) {
            if(do_later_map[pos].id == id) break; // found it
        }
        if(pos < do_later_map_count) { // loop didn't run out and found
            do_later_delete(pos); // make space for replacement
        }
    }
    if(do_later_is_full()) { // no space, forget oldest (in beginning)
        // forget oldest
        do_later_delete(0);
        forgot_one = true;
    }
    // find position to insert
    for(pos=0; pos<do_later_map_count; pos++) {
        unsigned long delta = do_later_map[pos].time-current;
        // be aware of overrun
        if(delta >= in_ms && delta <= ULNOIOT_MAX_DO_LATER_INTERVAL) break; // found something scheduled later
    }
    // do actual insert at pos
    if(pos<do_later_map_count) {
        // move existing elements up
        memmove(do_later_map+pos+1, do_later_map+pos, sizeof(do_later_map_t)*(do_later_map_count-pos-1));
    }
    do_later_map[pos].time = in_ms;
    do_later_map[pos].id = id;
    do_later_map[pos].callback = cbu;
    do_later_map_count ++;
//    ulog("Adding do_later scheduler at %d with time %ld at time %ld",pos,in_ms,current); // TODO: remove debug
    return !forgot_one;
}

bool do_later(unsigned long in_ms, int16_t id, DO_LATER_CB_ID callback) {
    if(id==-1) {
        ulog("The id -1 is not allowed for id-based do_later callback.");
    } else {
        Do_Later_Callback cbu = [id,callback] { callback(id); };
        return do_later_add(in_ms, id, cbu);
    }
    return true;
}

bool do_later(unsigned long in_ms, DO_LATER_CB_NO_ID callback) {
    return do_later_add(in_ms, -1, callback);
}

void do_later_check() {
    unsigned long next;
    unsigned long current=millis();
    unsigned long delta;
    while(true) {
        if(do_later_map_count<=0) break;
        next = do_later_map[0].time;
        delta = current - next;
        // pay attention to overrun
        if(delta>=0 && delta <= ULNOIOT_MAX_DO_LATER_INTERVAL) {
            //int16_t id = do_later_map[0].id; hardcoded in callback lambda
            Do_Later_Callback cb = do_later_map[0].callback;
            // release entry as there might be a new one created
            do_later_delete(0);
            cb();
        } else {
            break; // we are done
        }
    }
}

////// Device list
static Fixed_Map<Device, ULNOIOT_MAX_DEVICES> 
    __attribute__((init_priority(65525))) device_list;

/*static Device* find_device(const Ustring& name) {
    return device_list.find(name);
}

static Device* find_device(const char* name) {
    return device_list.find(name);
}*/

bool add_device(Device &device) {
    ulog("add_device: Adding device: %s", device.get_name().as_cstr());
    return device_list.add(&device);
}

bool devices_start() {
    bool all_success = true;
    device_list.for_each( [&] (Device& dev) {
        dev.start();
        if(!dev.started()) {
            all_success = false;
        }
        return true; // Continue loop
    } );
    return all_success;
}


bool devices_update() {
    bool changed = false;
    device_list.for_each( [&] (Device& dev) {
        if(dev.started()) {
            if(dev.poll_measure()) {
                changed |= dev.check_changes();
            }
        }
        return true; // Continue loop
    } );
    return changed;
}

bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all) {
    bool published = false;
    bool first = true;
    device_list.for_each( [&] (Device& dev) {
        if(dev.started()) {
            if(publish_all || dev.needs_publishing()) {
                if(first) {
                    Serial.print("Publishing");
                }
                if(dev.publish(mqtt_client, node_topic)) {
                    if(!first) Serial.print("; ");
                    published = true;
                }
                if(first) {
                    first = false;
                }
            }
        }
        return true; // Continue loop
    } );
    if(!first && !published) {
        Serial.println(" nothing.");
        return true; // If you did send nothing at all, say this publish was successful
    }
    if(published) Serial.println(".");
    return published;
}


bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    return devices_publish(mqtt_client, node_topic, false); 
}


bool devices_subscribe(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    // subscribe to all devices that accept input
    Ustring topic;

    device_list.for_each( [&] (Device& dev) {
        if( dev.started() ) {
            dev.subdevices_for_each( [&] (Subdevice& sd) {
                if(sd.subscribed()) {
                    // construct full topic
                    topic.copy(node_topic);
                    topic.add("/");
                    topic.add(dev.get_name());
                    if(sd.get_name().length() > 0) {
                        topic.add("/");
                        topic.add(sd.get_name());
                    }
                    uint16_t packetIdSub = mqtt_client.subscribe(topic.as_cstr(), 0);
                    ulog("Subscribing to %s with id %d.", 
                        topic.as_cstr(), packetIdSub);
                }
                return true; // continue subdevice loop
            } ); // end foreach - iterate over subdevices
        } // endif dev.started()
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
                topic.copy(dev.get_name());
                if(sd.get_name().length()>0) {
                    topic.add("/");
                    topic.add(sd.get_name());
                }
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

