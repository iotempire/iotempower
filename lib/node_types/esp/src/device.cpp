/**
 * @file device.cpp
 * @brief Implementation of Device and Subdevice classes
 * @author ulno
 * @created 2018-07-16
 */

#include <Arduino.h>
#include "device.h"

/**
 * MQTT buffer size tracking
 * ==========================
 * Global variable to track the current MQTT buffer size.
 * Updated whenever a bigger big buffer is allocated.
 * Note: espMqttClient handles large payloads automatically, so this is mainly
 * for tracking and logging purposes.
 */
size_t iotempower_mqtt_buffer_size = 256;  // Default buffer size

/**
 * @brief Ensure MQTT buffer is large enough for the required size
 * 
 * This function is called when a big buffer is allocated to ensure the MQTT
 * client can send the data in a single packet. It adds overhead for MQTT
 * topic length and protocol headers.
 * 
 * @param required_size The size of data that needs to be sent via MQTT
 */
void iotempower_ensure_mqtt_buffer_size(size_t required_size) {
    // Add overhead for MQTT packet: topic (up to 128 bytes) + protocol overhead (~10 bytes)
    const size_t mqtt_overhead = 256;
    size_t needed_size = required_size + mqtt_overhead;
    
    if (needed_size > iotempower_mqtt_buffer_size) {
        iotempower_mqtt_buffer_size = needed_size;
        ulog(F("MQTT buffer size increased to %d bytes for big buffer support"), iotempower_mqtt_buffer_size);
    }
}

void Subdevice::init_log() {
    ulog(F("subdevice init: subname: >%s<"), name.as_cstr()); 
}

void Subdevice::init_common(bool subscribed, size_t big_buffer_size) {
    _subscribed = subscribed;
    _big_buffer_size = big_buffer_size;
    _big_buffer_filled = 0;
    if (_big_buffer_size > 0) {
        _big_buffer = (uint8_t*) malloc(_big_buffer_size);
        if (_big_buffer == NULL) {
            ulog(F("Failed to allocate big buffer of size %d"), _big_buffer_size);
            _big_buffer_size = 0;
        } else {
            // Successfully allocated big buffer - ensure MQTT can handle it
            iotempower_ensure_mqtt_buffer_size(_big_buffer_size);
        }
    }
    init_log();
}

void Subdevice::init(bool subscribed, size_t big_buffer_size) {
    name.clear();
    init_common(subscribed, big_buffer_size);
}

void Subdevice::init(const char* subname, bool subscribed, size_t big_buffer_size) {
    name.from(subname);
    init_common(subscribed, big_buffer_size);
}

void Subdevice::init(const __FlashStringHelper* subname, bool subscribed, size_t big_buffer_size) {
    name.from(subname);
    init_common(subscribed, big_buffer_size);
}

bool Subdevice::call_receive_cb(Ustring& payload) {
    ulog(F("Calling receive callback >%s< >%s<"), 
        name.as_cstr(), payload.as_cstr());
    if(receive_cb != NULL) {
        return receive_cb(payload);
    }
    ulog(F("!failure receive_cb!"));
    return false;
}

#include "device-manager.h"

/**
 * @brief Constructor - registers device with DeviceManager automatically
 */
Device::Device(const char* _name, unsigned long __pollrate_us) {
    name.from(_name);
    pollrate_us(__pollrate_us);
    ulog(F("Device: Adding device: %s pollrate: %lu"), name.as_cstr(), _pollrate_us);
    if(!device_manager.add(*this)) {
        ulog(F("Device %s couldn't be added - max devices reached."), _name);
    }
}

#ifdef mqtt_discovery_prefix
void Device::create_discovery_info(const String& type,
        bool state_topic, const char* state_on, const char* state_off,
        bool command_topic, const char* payload_on, const char* payload_off,
        const String& extra_json) { // TODO: for inverted on/off this doesn't really work -> rethink
    
// should work dynamically  
//    discovery_config_topic.reserve(128); // extract lengths into constants
//    discovery_info.reserve(512);

    String slash(F("/"));
    String qmarks(F("\""));
    String comma_marks(F(", \""));
    String name_s(name.as_cstr());
    String topic_s = String(mqtt_topic) + slash + name_s;
    String hostname_s(HOSTNAME);

    discovery_config_topic = String(F(mqtt_discovery_prefix)) + slash + type + slash
        + hostname_s + slash + name_s + String(F("/config"));

    discovery_info = String(F("{ \"name\": \"")) + hostname_s + String(F(" ")) + String(name.as_cstr()) + qmarks;
    if(state_topic) {
        discovery_info += comma_marks + String(F("state_topic\": \"")) + topic_s + qmarks;
    }
    if(state_on)
        discovery_info += comma_marks + String(F("state_on\": \"")) + String(state_on) + qmarks;
    if(state_off)
        discovery_info += comma_marks + String(F("state_off\": \"")) + String(state_off) + qmarks;
    if(command_topic) {
        discovery_info += comma_marks + String(F("command_topic\": \"")) 
                            + topic_s + String(F("/set")) + qmarks;
    }
    if(payload_on)
        discovery_info += comma_marks + String(F("payload_on\": \"")) + String(payload_on) + qmarks;
    if(payload_off)
        discovery_info += comma_marks + String(F("payload_off\": \"")) + String(payload_off) + qmarks;
    if(extra_json.length() > 0) {
        discovery_info += F(", ");
        discovery_info += extra_json;
    }
    discovery_info += F(" }");
    
    // Serial.print(F("discovery topic: "));
    // Serial.println(discovery_config_topic);
    // Serial.print(F("discovery info: "));
    // Serial.println(discovery_info);
}
#endif

/**
 * @brief Publish device values to MQTT - constructs topics and sends values
 */
bool Device::publish(espMqttClient& mqtt_client, Ustring& node_topic, Ustring& log_buffer) {
    bool published = false;
    Ustring topic;
    bool first = true;
    subdevices_for_each( [&] (Subdevice& sd) {
        // Check if there's a big buffer to publish
        bool has_data = false;
        const uint8_t* data_ptr = NULL;
        size_t data_len = 0;
        
        if (sd.has_big_buffer()) {
            // Publish big buffer data only if filled > 0
            size_t filled = sd.get_big_buffer_filled();
            if (filled > 0) {
                data_ptr = sd.get_big_buffer();
                data_len = filled;  // Only publish filled portion
                has_data = true;
            }
        } else {
            // Publish string value
            const Ustring& val = sd.get_last_confirmed_value();
            if (!val.empty()) {
                data_ptr = (const uint8_t*) val.as_cstr();
                data_len = val.length();
                has_data = true;
            }
        }
        
        if (has_data) {
            // construct full topic
            topic.copy(node_topic);
            topic.add(F("/"));
            topic.add(get_name());
            const Ustring& sd_name = sd.get_name();
            if(!sd_name.empty()) { // only add, if name given
                topic.add(F("/"));
                topic.add(sd.get_name());
            }
            if(first) {
                log_buffer.add(F(" "));
                first = false;
            }
            else log_buffer.add(F("|"));
            log_buffer.add(topic);
            log_buffer.add(F(":"));
            if (sd.has_big_buffer()) {
                log_buffer.add(F("binary<"));
                log_buffer.add(data_len);
                log_buffer.add(F(">"));
            } else {
                log_buffer.add(sd.get_last_confirmed_value());
            }

            // // DEBUG: check how long mqtt publish takes
            // unsigned long publish_start = micros();
 
            // Publish with espMqttClient
            yield();
            
            uint16_t packet_id = mqtt_client.publish(topic.as_cstr(), 0, _retained, data_ptr, data_len);
 
            if(packet_id == 0) {
                ulog(F("DEBUG: publish error!"));
                log_buffer.add(F("!publish error!"));
                // TODO: signal error and trigger reconnect - necessary?
                return false;
            }
            
            yield();
            published = true;
            
            // After successful publish, clear big buffer for next recording
            if (sd.has_big_buffer()) {
                sd.big_buffer_clear();
            }
 
            // unsigned long publish_end = micros();
            // unsigned long publish_duration = publish_end - publish_start;
            // ulog(F("Published to topic %s, packet_id: %d, duration: %lu us"), topic.as_cstr(), packet_id, publish_duration);
 
        }
        return true; // continue loop
    } ); // end for_each - iterate over subdevices
    if(published) _needs_publishing = false;
    return published;
}

#ifdef mqtt_discovery_prefix
bool Device::publish_discovery_info(espMqttClient& mqtt_client) {
    if(discovery_config_topic.length()>0) { // only if exists
        ulog(F("Publishing discovery info for %s."), name.as_cstr());
        // espMqttClient handles larger payloads automatically
        // API: uint16_t publish(const char* topic, uint8_t qos, bool retain, const uint8_t* payload, size_t length)
        uint16_t packet_id = mqtt_client.publish(discovery_config_topic.c_str(), 0, true, 
                                                  (const uint8_t*)discovery_info.c_str(), 
                                                  discovery_info.length());
        if(packet_id == 0) {
            ulog(F("!discovery publish error!"));
            return false;
        }
    }
    return true;
}
#endif



/**
 * @brief Poll device if poll interval elapsed - calls measure() and runs filters
 */
bool Device::poll_measure() {
    if(started()) { // only if device active
        unsigned long current_micros = micros();

        if (current_micros - last_poll >= _pollrate_us) {
            // Calculate the number of _pollrate_us intervals that have passed

            unsigned long intervals = 0;
            if(_pollrate_us) intervals = (current_micros - last_poll) / _pollrate_us; // only update if not zero - TODO: verify that this cannot be more efficient

            // Update last_poll by adding the total amount of passed intervals
            last_poll += intervals * _pollrate_us;

            // // Optionally log if intervals > 1, meaning some measures were skipped
            // if (intervals > 1) {
            //     ulog(F("Skipping %u measures of %u"), intervals, _pollrate_us);
            //     // TODO: Note: Ensure logging does not take too long or consider removing it
            // }

            measure_init(); // something might have to be executed here to init each time, for example i2c-setup
            bool measure_result = measure(); // measure new value or trigger physical update
//            ulog("poll measure val: %s, %lu", name.as_cstr(), _pollrate_us); // TODO: enable based on debug level
//            yield(); // measure might have taken long, give some time to system TODO: do we have to take care of mqtt here too? - only yield in unprecision interval
            measure_exit(); // something might have to be executed here to exit each time, for example i2c-exit
            if(measure_result) { // new value generated
                if(_ignore_case) { // if necessary, lower them all
                    subdevices.for_each( [](Subdevice& sd) {
                        sd.measured_value.lower();
                        return true; // continue loop
                    } );
                }
                // check if it needs to be filtered - only filter when new value was measured
                if(filter_first != NULL && ! filter_first->call()) {
                    // if filter defined but indicates to ignore the current measurement
                    return false; // end here with no update
                }
            } else { // no new value generated
                // re-use last value(s), if measurement not successful
                subdevices.for_each( [](Subdevice& sd) {
                    sd.measured_value.copy(sd.last_confirmed_value);
                    return true; // continue loop
                } );
            }
            
            // The measured value(s) is/are now an actual, valid new measurement (or last value)
            return true;
        }
    }
    return false;
}

/**
 * @brief Check if values changed, call on_change callbacks, mark for publishing
 */
bool Device::check_changes() {
    // check if value has changed/is updated and call on_change_callback if it did
    
    // Check if there was a change in any subdevice
    bool changed = false;
    subdevices.for_each( [&] (Subdevice& sd) {
        // For big buffers, check if filled > 0 (has new data)
        if (sd.has_big_buffer()) {
            if (sd.get_big_buffer_filled() > 0) {
                changed = true;
                return false; // stop loop after one change found
            }
        }
        if(!sd.measured_value.equals(sd.last_confirmed_value)) {
            changed = true;
            return false; // stop loop after one change found
        }
        return true; // continue loop
    } ); // end for each subdevices

    // Call on_change callback if change happened
    bool updated = false;
    if(changed) {
        if(call_on_change_callbacks()) {
            subdevices.for_each( [&] (Subdevice& sd) {
                // For big buffers, check if filled > 0 (has new data to publish)
                bool needs_update = (sd.has_big_buffer() && sd.get_big_buffer_filled() > 0) 
                                    || !sd.measured_value.equals(sd.last_confirmed_value);
                
                if(needs_update) {
                    if(_report_change) {
                        updated = true;
                        _needs_publishing = true;
                        Ustring log_buffer(F("Needs publishing: "));
                        log_buffer.add(name);
                        if(!sd.get_name().empty()) {
                            log_buffer.add(F("/"));
                            log_buffer.add(sd.get_name());
                        }
                        log_buffer.add(F(":"));
                        if (sd.has_big_buffer()) {
                            log_buffer.add(F("binary<"));
                            log_buffer.add(sd.get_big_buffer_filled());
                            log_buffer.add(F(">"));
                        } else {
                            log_buffer.add(sd.measured_value);
                        }
                        ulog(log_buffer.as_cstr());
                    }
                    // Only copy string values, not big buffers (they're managed separately)
                    if (!sd.has_big_buffer()) {
                        sd.last_confirmed_value.copy(sd.measured_value);
                    }
                }
                return true; // continue whole loop to copy confirmed values
            } ); // end for each subdevices
        }
    } // changed?
    return updated;
}

static Ustring value_error;

// TODO: why can't I make this const/const?
Ustring& Device::get_last_confirmed_value(unsigned long index) {
    Subdevice* sd = subdevice(index);
    if(sd == NULL) {
        ulog(F("Error in get_last_confirmed_value(). Device: %s"), name.as_cstr());
        value_error.from(F("subdevice value error"));
        return value_error;
        // TODO: should this crash here?
    }
    return sd->get_last_confirmed_value();
}

Ustring& Device::value(unsigned long index) {
    Subdevice* sd = subdevice(index);
    if(sd == NULL) {
        ulog(F("Error in value(): Device: %s"), name.as_cstr());
        value_error.from(F("no subdevice"));
        return value_error;
    }
    return sd->measured_value;
}

void Device::clear_all_values() {
    for(unsigned int i = 0; i < subdevices_count(); i++) {
        value(i).clear();
    }
}
