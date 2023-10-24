// device-manager.cpp
// author: ulno
// created: 2018-07-16

#include <Arduino.h>
////AsyncMqttClient disabled in favor of PubSubClient
//#include <AsyncMqttClient.h>
#include <PubSubClient.h>
#include <toolbox.h>
#include <device-manager.h>

////// structure and list for scheduler
typedef std::function<void()> Do_Later_Callback;

typedef struct {
    unsigned long time;
    int16_t id;
    Do_Later_Callback callback;
} do_later_map_t;

static do_later_map_t do_later_map[IOTEMPOWER_DO_LATER_MAP_SIZE];
static int do_later_map_count=0;

bool do_later_is_full() {
    return do_later_map_count >= IOTEMPOWER_DO_LATER_MAP_SIZE;
}

static void do_later_delete(int pos) {
    if(do_later_map_count<=0) return;
    pos = limit(pos, 0, do_later_map_count-1);
//    ulog(F("do_later_delete pos: %d  count: %d"),pos,do_later_map_count);
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
        if(delta >= in_ms && delta <= IOTEMPOWER_MAX_DO_LATER_INTERVAL) break; // found something scheduled later
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
//    ulog(F("Adding do_later scheduler at %d with time %ld at time %ld"),pos,in_ms,current); // TODO: remove debug
    return !forgot_one;
}

bool do_later(unsigned long in_ms, int16_t id, DO_LATER_CB_ID callback) {
    if(id==-1) {
        ulog(F("The id -1 is not allowed for id-based do_later callback."));
    } else {
        Do_Later_Callback cbu = [id,callback] { callback(id); };
        return do_later_add(in_ms, id, cbu);
    }
    return true;
}

void deep_sleep(unsigned long time_from_now_ms, unsigned long duration_ms) {
    static unsigned long duration_ms_backup;

    duration_ms_backup = duration_ms; // static to save for lambda
    do_later(time_from_now_ms, [&] {
        ulog(F("About to fall into deepsleep for %lu s"), duration_ms_backup/1000);
        delay(100);
        delay(100);
        delay(100);
        ESP.deepSleep(duration_ms_backup*1000); 
    });
    // TODO: make sure, that at wake-up time the 5s reset is not triggered
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
        if(delta>=0 && delta <= IOTEMPOWER_MAX_DO_LATER_INTERVAL) {
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

// ////// Device list - now singleton
// static Fixed_Map<Device, IOTEMPOWER_MAX_DEVICES> 
//     __attribute__((init_priority(65525))) device_list;

// TODO: timestack, do_later


DeviceManager& DeviceManager::get_instance() {
    static DeviceManager instance;
    return instance;
}

DeviceManager::DeviceManager() {
    // Constructor code, if needed
}

DeviceManager::~DeviceManager() {
    // Destructor code, if needed
}

void DeviceManager::log_length() {
    ulog("Number of devices: %d", device_list.length());
}

bool DeviceManager::add(Device &device) {
    ulog(F("Adding device: Adding device: %s"), device.get_name().as_cstr());
    bool retval = device_list.add(&device);
    return retval;
}

bool DeviceManager::start() {
    bool all_success = true;
    device_list.for_each( [&] (Device& dev) {
        // dev.measure_init(); // init eventually i2c -> happens now if necessary in start
        dev.start();
        if(!dev.started()) {
            all_success = false;
        }
        return true; // Continue loop
    } );
    return all_success;
}

bool DeviceManager::update() {
    bool changed = false;
    device_list.for_each( [&] (Device& dev) {
        if(dev.poll_measure()) {
            changed |= dev.check_changes();
        }
        return true; // Continue loop
    } );
    return changed;
}

bool DeviceManager::publish(PubSubClient& mqtt_client, Ustring& node_topic, bool publish_all) {
    ////AsyncMqttClient disabled in favor of PubSubClient
    //bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all) {
    bool published = false;
    bool first = true;
    Ustring log_buffer(F("Publishing"));
    device_list.for_each( [&] (Device& dev) {
        if(dev.started()) {
            if(dev.get_report_change() && (publish_all || dev.needs_publishing())) {
                if(dev.publish(mqtt_client, node_topic, log_buffer)) {
                    if(!first) log_buffer.add(F("; "));
                    published = true;
                }
                mqtt_client.loop(); // give time to send things out -> seems necessary
                if(first) {
                    first = false;
                }
            }
        }
        return true; // Continue loop
    } );
    if(!first && !published) {
        log_buffer.add(F(" nothing"));
        published = true; // If you did send nothing at all, say this publish was successful
    } else {
        if(log_buffer.length() > 10) { // more than "Publishing."
            ulog(log_buffer.as_cstr());
        }
    }
    return published;
}

bool DeviceManager::subscribe(PubSubClient& mqtt_client, Ustring& node_topic) {
    ////AsyncMqttClient disabled in favor of PubSubClient
    //bool devices_subscribe(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
    // subscribe to all devices that accept input
    Ustring topic;

    device_list.for_each( [&] (Device& dev) {
        if( dev.started() ) {
            dev.subdevices_for_each( [&] (Subdevice& sd) {
                ulog("device: %s subdevice: %s", node_topic.as_cstr(),  sd.get_name().as_cstr()); // TODO: remove
                if(sd.subscribed()) {
                    // construct full topic
                    topic.copy(node_topic);
                    topic.add(F("/"));
                    topic.add(dev.get_name());
                    if(sd.get_name().length() > 0) {
                        topic.add(F("/"));
                        topic.add(sd.get_name());
                    }
                    uint16_t packetIdSub = mqtt_client.subscribe(topic.as_cstr(), 0);
                    ulog(F("Subscribing to %s with id %d."), 
                        topic.as_cstr(), packetIdSub);
                }
                return true; // continue subdevice loop
            } ); // end foreach - iterate over subdevices
        } // endif dev.started()
        return true; //continue loop device loop
    } ); // end foreach - iterate over devices
    return true; // TODO: decide if error occurred
}

bool DeviceManager::publish_discovery_info(PubSubClient& mqtt_client) {
    ////AsyncMqttClient disabled in favor of PubSubClient
    //bool devices_publish_discovery_info(AsyncMqttClient& mqtt_client) {

    #ifdef mqtt_discovery_prefix

        device_list.for_each( [&] (Device& dev) {
            if( dev.started() ) {
                dev.publish_discovery_info(mqtt_client);
            } // endif dev.started()
            return true; //continue loop device loop
        } ); // end foreach - iterate over devices
        
    #endif

    return true; // TODO: decide if error ocurred
}


bool DeviceManager::receive(Ustring& subtopic, Ustring& payload) {
    Ustring topic;

    // find matching subdevice for this topic/payload
    // subtopic is without node_topic
    device_list.for_each( [&] (Device& dev) {
        dev.subdevices_for_each( [&] (Subdevice& sd) {
            if(sd.subscribed()) {
                // construct full topic
                topic.copy(dev.get_name());
                if(sd.get_name().length()>0) {
                    topic.add(F("/"));
                    topic.add(sd.get_name());
                }
                // Serial.print(F("Trying to match "));
                // Serial.print(subtopic.as_cstr());
                // Serial.print(F(" with "));
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

void DeviceManager::get_report_list(Fixed_Buffer& b) {
    device_list.for_each( [&] (Device& dev) {
        b.append(dev.get_name().length(), (byte*)dev.get_name().as_cstr());
        byte sdc = dev.subdevices_count();
        b.append_nolen(1, &sdc);
        dev.subdevices_for_each( [&] (Subdevice& sd) {
            b.append(sd.get_name().length(), (byte*)sd.get_name().as_cstr());
            return true; // Continue loop
        } ); // subdevices
        return true; // Continue loop
    } ); // devices
}

/*static Device* find_device(const Ustring& name) {
    return device_list.find(name);
}

static Device* find_device(const char* name) {
    return device_list.find(name);
}*/