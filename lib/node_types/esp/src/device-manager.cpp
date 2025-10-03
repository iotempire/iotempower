/**
 * @file device-manager.cpp
 * @brief Implementation of DeviceManager and do_later scheduler
 * @author ulno
 * @created 2018-07-16
 * 
 * IMPLEMENTATION OVERVIEW
 * =======================
 * This file implements:
 * 1. The do_later scheduler for delayed callback execution
 * 2. The DeviceManager singleton for managing all devices
 * 3. Device lifecycle operations (start, update, publish)
 * 4. MQTT integration for all devices
 * 
 * DO_LATER SCHEDULER IMPLEMENTATION
 * ==================================
 * The scheduler maintains a sorted array of callbacks to execute at specific times.
 * 
 * Data structure:
 * - Array of {time, id, callback} entries
 * - Sorted by execution time (earliest first)
 * - Fixed maximum size (IOTEMPOWER_DO_LATER_MAP_SIZE = 64)
 * 
 * Key operations:
 * - Insert: Binary search for correct position, shift array
 * - Delete: Shift array to fill gap
 * - Check: Execute callbacks whose time has come
 * 
 * Time handling:
 * - Uses millis() which overflows every ~49 days
 * - Handles overflow with delta calculations
 * - Maximum delay: 24 hours (prevents overflow issues)
 * 
 * ID-based callbacks:
 * - If id != -1, replaces any existing callback with same id
 * - Useful for debouncing (new action cancels old one)
 * - Example: Button press schedules action, repeated presses update timing
 * 
 * DEVICEMANAGER IMPLEMENTATION
 * ============================
 * Singleton pattern ensures single device registry.
 * Uses Fixed_Map for efficient device lookup by name.
 * All operations iterate through device list using lambda callbacks.
 */

#include <Arduino.h>
////AsyncMqttClient disabled in favor of PubSubClient
//#include <AsyncMqttClient.h>
#include <PubSubClient.h>
#include <toolbox.h>
#include <device-manager.h>

/**
 * DO_LATER SCHEDULER DATA STRUCTURES
 * ===================================
 */

/// Callback wrapper that can be stored (std::function supports lambdas)
typedef std::function<void()> Do_Later_Callback;

/// Scheduled callback entry
typedef struct {
    unsigned long time;         ///< millis() when callback should execute
    int16_t id;                ///< ID for replaceable callbacks (-1 for one-shot)
    Do_Later_Callback callback; ///< Function to call
} do_later_map_t;

/// Scheduler storage (sorted array, earliest callback first)
static do_later_map_t do_later_map[IOTEMPOWER_DO_LATER_MAP_SIZE];
static int do_later_map_count=0;  ///< Number of scheduled callbacks

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

/**
 * @brief Add a callback to the scheduler (internal function)
 * 
 * SCHEDULER INSERTION ALGORITHM
 * ==============================
 * This function implements a sorted insertion algorithm:
 * 
 * 1. If callback has an ID, check if one with same ID exists:
 *    - If found, delete old callback (replacement)
 *    - This implements debouncing/replacement logic
 * 
 * 2. If scheduler is full:
 *    - Delete oldest callback (at position 0)
 *    - This ensures newer callbacks have priority
 * 
 * 3. Find insertion position:
 *    - Linear search through sorted array
 *    - Uses delta calculation to handle millis() overflow
 *    - Insert before first callback scheduled for later
 * 
 * 4. Insert callback:
 *    - Shift existing callbacks to make room
 *    - Insert new callback at correct position
 *    - Increment counter
 * 
 * OVERFLOW HANDLING
 * =================
 * millis() overflows every ~49 days. This is handled by:
 * - Using delta (difference) calculations
 * - Checking if delta is within valid range
 * - Maximum delay prevents wraparound issues
 * 
 * Example with overflow:
 *   current = 4294967290 (5ms before overflow)
 *   in_ms = 10ms from now
 *   time = 4294967300 (would overflow to 5)
 *   When checking: delta = 5 - 4294967290 = large negative (wraps to large positive)
 *   If delta > IOTEMPOWER_MAX_DO_LATER_INTERVAL, it's invalid
 * 
 * @param in_ms Delay in milliseconds
 * @param id Callback ID (-1 for one-shot, >=0 for replaceable)
 * @param cbu Callback function to execute
 * @return true if added successfully, false if had to forget oldest callback
 */
static bool do_later_add(unsigned long in_ms, int16_t id, Do_Later_Callback cbu) {
    bool forgot_one = false;
    // Insert sorted into list
    unsigned long current = millis();
    in_ms += current;  // Convert relative time to absolute time
    int pos; // Position variable for finding things
    
    // If valid id (!=-1) is given, a potential existing position needs 
    // to be deleted (replacement logic)
    if(id!=-1) {
        // Check if already in there
        for(pos=0; pos<do_later_map_count; pos++) {
            if(do_later_map[pos].id == id) break; // Found it
        }
        if(pos < do_later_map_count) { // Loop didn't run out and found
            do_later_delete(pos); // Make space for replacement
        }
    }
    if(do_later_is_full()) { // No space, forget oldest (at beginning)
        // Forget oldest
        do_later_delete(0);
        forgot_one = true;
    }
    // Find position to insert (maintain sorted order)
    for(pos=0; pos<do_later_map_count; pos++) {
        unsigned long delta = do_later_map[pos].time-current;
        // Be aware of overflow - only valid if delta is reasonable
        if(delta >= in_ms && delta <= IOTEMPOWER_MAX_DO_LATER_INTERVAL) break; // Found something scheduled later
    }
    // Do actual insert at pos
    if(pos<do_later_map_count) {
        // Move existing elements up to make room
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

/**
 * @brief Check and execute ready callbacks (called from main loop)
 * 
 * SCHEDULER EXECUTION
 * ===================
 * This function is called in every iteration of the main loop.
 * It checks if any scheduled callbacks are ready to execute.
 * 
 * Algorithm:
 * 1. Check if scheduler is empty - if so, return immediately
 * 2. Get next callback (always at position 0 due to sorting)
 * 3. Calculate delta between current time and scheduled time
 * 4. If delta indicates time has come (considering overflow):
 *    - Extract callback from scheduler
 *    - Delete it from the queue (before executing!)
 *    - Execute the callback
 * 5. Repeat until no more ready callbacks
 * 
 * WHY DELETE BEFORE EXECUTING?
 * ============================
 * The callback is deleted from the queue before execution because:
 * - Callback might call do_later() itself
 * - This could modify the queue
 * - Deleting first ensures queue consistency
 * - Callback execution might take significant time
 * 
 * OVERFLOW HANDLING
 * =================
 * Uses same delta calculation as insertion:
 * - If current time >= scheduled time (considering overflow)
 * - And delta is within valid range
 * - Then callback is ready to execute
 * 
 * Example timeline:
 *   T=0: Schedule callback for T=1000
 *   T=500: Check - delta = 500-1000 = large negative (not ready)
 *   T=1000: Check - delta = 1000-1000 = 0 (ready!)
 *   T=1100: Check - delta = 1100-1000 = 100 (ready, late but still valid)
 * 
 * This is called from the main loop outside precision intervals
 * to avoid interfering with precise timing requirements.
 */
void do_later_check() {
    unsigned long next;
    unsigned long current=millis();
    unsigned long delta;
    while(true) {
        if(do_later_map_count<=0) break;  // No more callbacks
        next = do_later_map[0].time;      // Next callback time
        delta = current - next;           // Time difference
        // Pay attention to overflow
        if(delta>=0 && delta <= IOTEMPOWER_MAX_DO_LATER_INTERVAL) {
            //int16_t id = do_later_map[0].id; // ID is hardcoded in callback lambda
            Do_Later_Callback cb = do_later_map[0].callback;
            // Release entry BEFORE execution as callback might schedule new ones
            do_later_delete(0);
            cb();  // Execute the callback
        } else {
            break; // Next callback not ready yet, we are done
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

/**
 * @brief Update all devices (poll sensors, check for changes)
 * 
 * THE HEART OF IOTEMPOWER'S AUTOMATIC DEVICE MANAGEMENT
 * ======================================================
 * This method is called from the main loop and implements the core
 * device polling logic that makes IoTempower so easy to use.
 * 
 * DUAL TIMING MODE
 * ================
 * IoTempower operates in two timing modes:
 * 
 * 1. PRECISION INTERVAL (in_precision_interval = true)
 *    - Very tight timing, no yields
 *    - Only devices that need precision update
 *    - Used for LED strips, audio, etc.
 *    - Fast and uninterrupted
 * 
 * 2. REGULAR INTERVAL (in_precision_interval = false)
 *    - Normal operation with yields
 *    - All devices update
 *    - Allows network/system tasks to run
 *    - More forgiving timing
 * 
 * WHAT HAPPENS FOR EACH DEVICE
 * =============================
 * 1. Check if device needs precision matches current interval
 * 2. Call dev.poll_measure():
 *    - Checks if poll rate has elapsed
 *    - Calls measure() to read sensor
 *    - Runs filter callbacks
 *    - Returns true if new value
 * 3. If new value, call dev.check_changes():
 *    - Compares to last confirmed value
 *    - Runs on_change callbacks
 *    - Marks for publishing if changed
 * 4. Yield (if not in precision mode) to let network run
 * 
 * COMPARISON TO STANDARD ARDUINO
 * ==============================
 * Standard Arduino would require:
 *   void loop() {
 *     // Poll temperature sensor
 *     if (millis() - tempLastPoll >= 1000) {
 *       float temp = readTemp();
 *       if (temp != lastTemp) {
 *         mqtt.publish("temp", String(temp));
 *         lastTemp = temp;
 *       }
 *       tempLastPoll = millis();
 *     }
 *     
 *     // Poll button
 *     if (millis() - buttonLastPoll >= 50) {
 *       bool pressed = digitalRead(BUTTON);
 *       if (pressed != lastPressed) {
 *         mqtt.publish("button", pressed ? "on" : "off");
 *         lastPressed = pressed;
 *       }
 *       buttonLastPoll = millis();
 *     }
 *     
 *     // ...repeat for every device...
 *     yield(); // Don't forget this!
 *   }
 * 
 * With IoTempower:
 *   // Just define devices in setup
 *   // This one line replaces all the code above:
 *   device_manager.update(in_precision_interval);
 * 
 * @param in_precision_interval true for precision mode, false for regular
 * @return true if any device values changed
 */
bool DeviceManager::update(bool in_precision_interval) {
    bool changed = false;
    device_list.for_each( [&] (Device& dev) {
        if(dev.needs_precision() == in_precision_interval) { // Execute precision devices according to what interval is selected
            if(dev.poll_measure()) {
                changed |= dev.check_changes();
            }
            if (!in_precision_interval) yield(); // Give time if not in precision mode
        }
        return true; // Continue loop
    } );
    return changed;
}

void DeviceManager::reset_buffers() {
    device_list.for_each( [&] (Device& dev) {
        dev.buffer_reset();
        return true; // Continue loop
    } );
}

/**
 * @brief Publish changed device values to MQTT
 * 
 * AUTOMATIC MQTT PUBLISHING
 * =========================
 * This method demonstrates another key IoTempower feature: automatic publishing
 * of changed values without any user code needed.
 * 
 * PUBLISHING LOGIC
 * ================
 * For each device:
 * 1. Check if device is started
 * 2. Check if device should report changes
 * 3. Check if publish_all OR device needs publishing:
 *    - publish_all: Periodic full state report
 *    - needs_publishing: Value changed since last publish
 * 4. Call device.publish() to send to MQTT
 * 5. Call mqtt_client.loop() to ensure message is sent
 * 
 * TWO PUBLISHING MODES
 * ====================
 * 
 * 1. CHANGE-BASED (publish_all = false)
 *    - Only publish devices that have changed
 *    - Reduces MQTT traffic significantly
 *    - Most common mode (called in every loop)
 *    - Example: Button pressed -> publish immediately
 * 
 * 2. FULL STATE (publish_all = true)
 *    - Publish all devices regardless of changes
 *    - Ensures external systems have current state
 *    - Called periodically (e.g., every 5 minutes)
 *    - Recovers from broker restarts or lost messages
 * 
 * DETAILED LOGGING
 * ================
 * Builds a log message showing all published values:
 *   "Publishing topic1:value1 | topic2:value2 | topic3:value3"
 * 
 * This helps with debugging and monitoring:
 * - See what's being published
 * - Verify values are correct
 * - Track publishing frequency
 * 
 * COMPARISON TO STANDARD ARDUINO
 * ==============================
 * 
 * Standard Arduino:
 *   void publishDevices() {
 *     if (tempChanged) {
 *       mqtt.publish("node/temp", String(temp));
 *       tempChanged = false;
 *     }
 *     if (humidityChanged) {
 *       mqtt.publish("node/humidity", String(humidity));
 *       humidityChanged = false;
 *     }
 *     // ...repeat for every device...
 *   }
 * 
 * With IoTempower:
 *   device_manager.publish(mqtt_client, node_topic, false);
 *   // Handles all devices automatically!
 * 
 * NETWORK CONSIDERATIONS
 * ======================
 * - Calls mqtt_client.loop() after each publish
 * - Ensures messages are sent out promptly
 * - Prevents buffer overflow
 * - Allows network stack to process packets
 * 
 * @param mqtt_client PubSubClient instance
 * @param node_topic Base MQTT topic for this node
 * @param publish_all If true, publish all values; if false, only changed
 * @return true if any values were published
 */
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
                mqtt_client.loop(); // Give time to send things out -> seems necessary
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
        if(log_buffer.length() > 10) { // More than "Publishing."
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