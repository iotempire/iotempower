/**
 * @file device.cpp
 * @brief Implementation of Device and Subdevice classes
 * @author ulno
 * @created 2018-07-16
 * 
 * IMPLEMENTATION OVERVIEW
 * =======================
 * This file implements the core device functionality including:
 * - Device lifecycle management (construction, start, measure, publish)
 * - Subdevice value management
 * - MQTT publishing logic
 * - Change detection
 * - Home Assistant discovery
 * 
 * KEY IMPLEMENTATION DETAILS
 * ==========================
 * 
 * 1. AUTOMATIC REGISTRATION
 *    When a Device is constructed, it automatically registers itself with
 *    the DeviceManager singleton. This happens before setup() runs due to
 *    C++ static initialization order.
 * 
 * 2. POLLING MECHANISM
 *    poll_measure() is called from the main loop for each device.
 *    It checks if enough time has passed based on pollrate_us, then:
 *    - Calls measure_init() for any pre-measurement setup
 *    - Calls measure() which derived classes override
 *    - Calls measure_exit() for any post-measurement cleanup
 *    - Runs filter callbacks to validate the measurement
 *    - Only updates values if filters return true
 * 
 * 3. CHANGE DETECTION
 *    check_changes() compares measured_value to last_confirmed_value
 *    for each subdevice. If any changed:
 *    - Calls on_change callbacks
 *    - Marks device for publishing
 *    - Copies measured_value to last_confirmed_value
 * 
 * 4. MQTT PUBLISHING
 *    publish() iterates through all subdevices and:
 *    - Constructs full MQTT topic
 *    - Publishes last_confirmed_value
 *    - Handles retained flag
 *    - Provides detailed logging
 * 
 * 5. HOME ASSISTANT DISCOVERY
 *    create_discovery_info() builds JSON payloads for Home Assistant MQTT discovery.
 *    This allows devices to appear in Home Assistant automatically without
 *    manual configuration.
 */

#include <Arduino.h>
#include "device.h"

void Subdevice::init_log() {
    ulog(F("subdevice init: subname: >%s<"), name.as_cstr()); 
}

void Subdevice::init(bool subscribed) {
    name.clear();
    _subscribed = subscribed;
    init_log();
}

void Subdevice::init(const char* subname, bool subscribed) {
    name.from(subname);
    _subscribed = subscribed;
    init_log();
}

void Subdevice::init(const __FlashStringHelper* subname, bool subscribed) {
    name.from(subname);
    _subscribed = subscribed;
    init_log();
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
 * @brief Device constructor - automatically registers device with DeviceManager
 * 
 * AUTOMATIC REGISTRATION - A KEY IOTEMPOWER FEATURE
 * ==================================================
 * This constructor demonstrates one of IoTempower's most powerful features:
 * automatic device registration.
 * 
 * In a standard Arduino project, you would need to:
 * 1. Declare device objects
 * 2. Manually keep track of all devices in an array or list
 * 3. Manually initialize each device in setup()
 * 4. Manually poll each device in loop()
 * 5. Manually publish each device's values
 * 
 * With IoTempower:
 * 1. Just create the device object
 * 2. Everything else is automatic!
 * 
 * HOW IT WORKS
 * ============
 * During construction:
 * 1. Device name is stored (used for MQTT topic)
 * 2. Poll rate is set (how often to read sensor)
 * 3. Device registers itself with DeviceManager
 * 4. DeviceManager will call start(), measure(), and publish() automatically
 * 
 * This happens during C++ static initialization, before setup() runs.
 * That's why you can declare devices globally and they "just work".
 * 
 * @param _name Device name (becomes part of MQTT topic)
 * @param __pollrate_us How often to poll this device in microseconds
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

////AsyncMqttClient disabled in favor of PubSubClient
//bool Device::publish(AsyncMqttClient& mqtt_client, Ustring& node_topic) {
// As we cannot use AsyncMqttClient (too many conflicts with other libraries that use interrupts)
// and being archived, we need to assuem for this publish process that no new sensor data will
// be acquired while publishing.
/**
 * @brief Publish device values to MQTT
 * 
 * AUTOMATIC MQTT TOPIC CONSTRUCTION
 * ==================================
 * One of IoTempower's key features is automatic MQTT topic generation.
 * 
 * Topic structure:
 * - Device with no subdevices: <node_topic>/<device_name>
 * - Device with subdevices: <node_topic>/<device_name>/<subdevice_name>
 * 
 * Example:
 * - Node topic: "living_room"
 * - Device name: "temperature"
 * - Subdevice name: "celsius"
 * - Full topic: "living_room/temperature/celsius"
 * 
 * In a standard MQTT project, you would need to:
 * 1. Manually construct topic strings
 * 2. Format payload data
 * 3. Call MQTT publish
 * 4. Handle errors and retries
 * 5. Manage retained flags
 * 
 * With IoTempower, all of this is automatic. Just set device values and
 * publishing happens automatically when values change.
 * 
 * PUBLISHING STRATEGY
 * ===================
 * - Iterates through all subdevices
 * - Publishes only non-empty values
 * - Constructs full topic path
 * - Logs all published values
 * - Yields to network stack between publishes
 * - Clears needs_publishing flag on success
 * 
 * @param mqtt_client PubSubClient instance
 * @param node_topic Base topic for this node
 * @param log_buffer Buffer for logging published values
 * @return true if any values were published
 */
bool Device::publish(PubSubClient& mqtt_client, Ustring& node_topic, Ustring& log_buffer) {
    bool published = false;
    Ustring topic;
    bool first = true;
    subdevices_for_each( [&] (Subdevice& sd) {
        const Ustring& val = sd.get_last_confirmed_value();
        if(!val.empty()) {
            // Construct full MQTT topic
            topic.copy(node_topic);
            topic.add(F("/"));
            topic.add(get_name());
            const Ustring& sd_name = sd.get_name();
            if(!sd_name.empty()) { // Only add subdevice name if it exists
                topic.add(F("/"));
                topic.add(sd.get_name());
            }
            if(first) {
                log_buffer.add(F(" "));
                first = false;
            }
            else log_buffer.add(F("|"));
// TODO: seems wrong, check if fixed correctly            Serial.print(topic.as_cstr()+node_topic.length()+1);
            log_buffer.add(topic);
            log_buffer.add(F(":"));
            log_buffer.add(val);

            ////AsyncMqttClient disabled in favor of PubSubClient
            // if(!mqtt_client.publish(topic.as_cstr(), 0, false, val.as_cstr())) {
            
            // Get buffers a bit emptier before we try to publish
            yield();
            mqtt_client.loop();
            yield();
            if(!mqtt_client.publish(topic.as_cstr(), (uint8_t*) val.as_cstr(), (unsigned int)val.length(), _retained)) {
                log_buffer.add(F("!publish error!"));
                // TODO: signal error and trigger reconnect - necessary?
                return false;
            }
            // Give room for publish to be sent out TODO: implement sync send
            yield();
            mqtt_client.loop();
            yield();
            published = true;
        }
        return true; // Continue loop
    } ); // end for_each - iterate over subdevices
    if(published) _needs_publishing = false;
    return published;
}

#ifdef mqtt_discovery_prefix
////AsyncMqttClient disabled in favor of PubSubClient
//bool Device::publish_discovery_info(AsyncMqttClient& mqtt_client) {
bool Device::publish_discovery_info(PubSubClient& mqtt_client) {
    if(discovery_config_topic.length()>0) { // only if exists
        ulog(F("Publishing discovery info for %s."), name.as_cstr());
        // TODO: check if retained and/or some cleanup necessary
        ////AsyncMqttClient disabled in favor of PubSubClient
        // if(!mqtt_client.publish(discovery_config_topic.c_str(), 0, true, discovery_info.c_str(), discovery_info.length())) {
        //     ulog(F("!discovery publish error!"));
        //     // TODO: signal error and trigger reconnect - necessary?
        //     return false;
        // } else {
        //     // make sure it gets sent
        //     delay(10);  // This delay is important to prevent overflow of network buffer TODO: implement sync publish mechanism
        // }
        unsigned int left = discovery_info.length();
        if(!mqtt_client.beginPublish(discovery_config_topic.c_str(), left, true)) {
            ulog(F("!discovery publish init error!"));
        }
        const uint8_t* start = (uint8_t*) discovery_info.c_str();
        while(true) {
            if(left>128) {
                if(!mqtt_client.write(start, 128)) break;
                start += 128;
                left -= 128;
            } else {
                if(!mqtt_client.write(start, left)) break;
                left = 0;
                break;
            }
        }
        if(left>0) {
            ulog(F("!discovery publish write error!"));
        }
        if(!mqtt_client.endPublish()) {
            ulog(F("!discovery publish end error!"));
            return false;
        }
    }
    return true;
}
#endif



/**
 * @brief Poll and measure device if enough time has passed
 * 
 * THE POLLING AND MEASUREMENT SYSTEM
 * ===================================
 * This is a core part of IoTempower's automatic device management.
 * 
 * HOW POLLING WORKS
 * =================
 * 1. Check if device is started
 * 2. Check if pollrate_us has elapsed since last_poll
 * 3. Calculate how many intervals passed (handles missed intervals)
 * 4. Call measure_init() for any pre-measurement setup (e.g., I2C bus setup)
 * 5. Call measure() to read sensor or update actuator
 * 6. Call measure_exit() for any post-measurement cleanup
 * 7. Run filter callbacks to validate measurement
 * 8. If filters pass, accept the new value
 * 9. If filters fail, revert to last confirmed value
 * 
 * COMPARISON TO STANDARD ARDUINO
 * ==============================
 * Standard Arduino:
 *   void loop() {
 *     if (millis() - lastRead > 1000) {
 *       temp = readTemperature();
 *       if (temp != lastTemp) {
 *         mqtt.publish("temp", String(temp));
 *         lastTemp = temp;
 *       }
 *       lastRead = millis();
 *     }
 *     // Repeat for every sensor...
 *   }
 * 
 * With IoTempower:
 *   // Just implement measure() in your device class
 *   bool measure() {
 *     value().from(readTemperature());
 *     return true;
 *   }
 *   // Everything else is automatic!
 * 
 * PRECISE TIMING
 * ==============
 * The timing is microsecond-accurate and handles:
 * - Missed intervals (if processing took too long)
 * - Drift compensation (uses intervals, not absolute time)
 * - Very fast polling (1000us = 1ms for LED strips)
 * - Very slow polling (1000000us = 1s for temperature)
 * 
 * @return true if measurement was taken and accepted
 */
bool Device::poll_measure() {
    if(started()) { // Only if device active
        unsigned long current_micros = micros();

        if (current_micros - last_poll >= _pollrate_us) {
            // Calculate the number of _pollrate_us intervals that have passed

            unsigned long intervals = 0;
            if(_pollrate_us) intervals = (current_micros - last_poll) / _pollrate_us; // Only update if not zero - TODO: verify that this cannot be more efficient

            // Update last_poll by adding the total amount of passed intervals
            last_poll += intervals * _pollrate_us;

            // // Optionally log if intervals > 1, meaning some measures were skipped
            // if (intervals > 1) {
            //     ulog(F("Skipping %u measures of %u"), intervals, _pollrate_us);
            //     // TODO: Note: Ensure logging does not take too long or consider removing it
            // }

            measure_init(); // something might have to be executed here to init each time, for example i2c-setup
            bool measure_result = measure(); // measure new value or trigger physical update
//            ulog("poll measure val: %s, %lu", name.as_cstr(), _pollrate_us); // TODO: remove/debug
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
 * @brief Check if values changed and trigger callbacks
 * 
 * AUTOMATIC CHANGE DETECTION
 * ===========================
 * This is another key IoTempower feature that eliminates boilerplate code.
 * 
 * In a standard Arduino project:
 *   float newTemp = readTemperature();
 *   if (newTemp != lastTemp) {
 *     mqtt.publish("temp", String(newTemp));
 *     lastTemp = newTemp;
 *   }
 *   // Repeat for every sensor...
 * 
 * With IoTempower:
 *   // Just write the value
 *   value().from(readTemperature());
 *   // Change detection, callbacks, and publishing are automatic!
 * 
 * HOW IT WORKS
 * ============
 * 1. Compare measured_value to last_confirmed_value for each subdevice
 * 2. If any changed, call on_change callbacks
 * 3. If callbacks accept the change (return true):
 *    - Mark device as needs_publishing
 *    - Log the change
 *    - Copy measured_value to last_confirmed_value
 * 4. If callbacks reject the change (return false):
 *    - Keep old value
 *    - Don't publish
 * 
 * This allows for:
 * - Automatic MQTT traffic reduction (only publish changes)
 * - Callback-based automation (trigger actions on changes)
 * - Change validation (reject suspicious changes)
 * - Detailed logging of what changed
 * 
 * @return true if any values changed and were accepted
 */
bool Device::check_changes() {
    // Check if value has changed/is updated and call on_change_callback if it did
    
    // Check if there was a change in any subdevice
    bool changed = false;
    subdevices.for_each( [&] (Subdevice& sd) {
        if(!sd.measured_value.equals(sd.last_confirmed_value)) {
            changed = true;
            return false; // Stop loop after one change found
        }
        return true; // Continue loop
    } ); // end for each subdevices

    // Call on_change callback if change happened
    bool updated = false;
    if(changed) {
        if(call_on_change_callbacks()) {
            subdevices.for_each( [&] (Subdevice& sd) {
                if(!sd.measured_value.equals(sd.last_confirmed_value)) {
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
                        log_buffer.add(sd.measured_value);
                        ulog(log_buffer.as_cstr());
                    }
                    sd.last_confirmed_value.copy(sd.measured_value);
                }
                return true; // Continue whole loop to copy confirmed values
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
