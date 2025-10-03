/**
 * @file device.h
 * @brief Base Device class for IoTempower - the foundation of the device system
 * @author ulno
 * @created 2018-07-16
 * 
 * WHAT IS A DEVICE IN IOTEMPOWER?
 * ================================
 * A Device represents any sensor or actuator connected to an IoT node. This could be:
 * - Sensors: temperature, humidity, light, motion, buttons, distance sensors, etc.
 * - Actuators: LEDs, relays, motors, servos, displays, etc.
 * 
 * CORE DESIGN PRINCIPLES
 * ======================
 * 
 * 1. STRING-BASED VALUES
 *    All device values are passed as strings (Ustring) for simplicity and consistency.
 *    This means:
 *    - Easy MQTT publishing (MQTT payloads are strings)
 *    - Simple debugging (can print any value)
 *    - Type conversion handled by helper methods (read_int, read_float, etc.)
 *    - No complex type marshalling needed
 * 
 * 2. AUTOMATIC REGISTRATION
 *    Devices register themselves with DeviceManager during construction.
 *    Users just create a device object and it's automatically:
 *    - Added to the device list
 *    - Given an MQTT topic
 *    - Set up for polling
 *    - Ready to publish and receive
 * 
 * 3. SUBDEVICE ARCHITECTURE
 *    A single Device can have multiple subdevices (e.g., DHT sensor has temp & humidity).
 *    Each subdevice:
 *    - Has its own name and MQTT subtopic
 *    - Can have its own value and callbacks
 *    - Can be subscribed to receive commands
 * 
 * 4. CALLBACK CHAINS
 *    Devices support two callback chains:
 *    - Filter callbacks: Process/validate measurements before they're accepted
 *    - Change callbacks: Trigger when a value changes
 *    Callbacks can be chained for complex processing pipelines
 * 
 * 5. LIFECYCLE MANAGEMENT
 *    Every device goes through a clear lifecycle:
 *    - Construction: Device created, registers with DeviceManager
 *    - start(): Initialize hardware, called by DeviceManager
 *    - measure(): Poll sensor/update actuator, called based on pollrate
 *    - publish(): Send values to MQTT when they change
 *    - receive(): Handle incoming MQTT commands
 * 
 * HOW THIS DIFFERS FROM STANDARD ARDUINO
 * =======================================
 * In a standard Arduino MQTT project, you would need:
 * - Manual sensor reading in loop() with timing logic
 * - Manual MQTT topic construction and message formatting
 * - Manual change detection (store old value, compare to new)
 * - Manual subscription and message routing
 * - Manual callback registration and management
 * 
 * With IoTempower Device class:
 * - Just inherit from Device and implement measure()
 * - Automatic polling based on pollrate
 * - Automatic MQTT topic generation
 * - Automatic change detection and publishing
 * - Automatic message routing to your device
 * - Built-in callback chains for filtering and triggering
 */

// General note: In IoTempower, we pass all values as strings (Ustring) for convenience.
// Other types must be converted to/from strings using the provided helper methods.
// We are actually using Ustring as defined in toolbox.h for memory efficiency.

#ifndef _IOTEMPOWER_DEVICE_H_
#define _IOTEMPOWER_DEVICE_H_

#include <functional>
////AsyncMqttClient disabled in favor of PubSubClient
//#include <AsyncMqttClient.h>
#include <PubSubClient.h>
#include <iotempower-default.h>
#include <toolbox.h>

#include "config-wrapper.h"

class Device;

/**
 * Type definition for device callbacks
 * Callbacks receive a reference to the device and return bool:
 * - true: Accept/process the value
 * - false: Reject/ignore the value
 */
#define IOTEMPOWER_DEVICE_CALLBACK std::function<bool(Device &dev)>

/**
 * @class Callback
 * @brief Wrapper for device callback functions
 * 
 * PURPOSE
 * =======
 * Provides a flexible callback mechanism that can:
 * - Store a function to be called later
 * - Be overridden by subclasses for custom behavior
 * - Access the device it's associated with
 * 
 * USAGE
 * =====
 * Callbacks are used for:
 * - Filter callbacks: Validate/modify measurements
 * - Change callbacks: React to value changes
 * - Command callbacks: Handle MQTT commands
 * 
 * The callback can either:
 * 1. Be initialized with a lambda/function
 * 2. Be subclassed and override the call() method
 */
class Callback {
    private:
        IOTEMPOWER_DEVICE_CALLBACK _callback = NULL;
        Device* _dev;
    public:
        Callback(IOTEMPOWER_DEVICE_CALLBACK callback = NULL) {
            _callback = callback;
        }
        void set_device(Device& dev) { _dev = &dev; }
        Device& get_device() { return *_dev; }
        virtual bool call(Device &dev) {
            if(_callback) return _callback(dev);
            return false; // if not overwritten, let this filter forget everything
        }
        bool call() { // This is mainly called from Callback Link to make sure that internal device is used
            return call(*_dev);
        }
};


/**
 * @class Callback_Link
 * @brief Linked list of callbacks for processing pipelines
 * 
 * PURPOSE
 * =======
 * Allows multiple callbacks to be chained together in a processing pipeline.
 * All callbacks in the chain must return true for the chain to succeed.
 * 
 * EXAMPLE USE CASES
 * =================
 * 1. Filter Chain:
 *    - First callback: Validate sensor reading is in valid range
 *    - Second callback: Apply calibration correction
 *    - Third callback: Average with previous readings
 * 
 * 2. Trigger Chain:
 *    - First callback: Check if temperature above threshold
 *    - Second callback: Send alert if not already sent
 *    - Third callback: Turn on cooling system
 * 
 * HOW IT WORKS
 * ============
 * - Each Callback_Link holds one Callback and a pointer to the next link
 * - Calling the chain calls each callback in sequence
 * - If any callback returns false, the chain stops and returns false
 * - Only if all callbacks return true does the chain return true
 * 
 * This is a key feature that makes IoTempower more powerful than standard
 * Arduino code - you can build complex processing logic by chaining simple
 * callbacks without writing explicit control flow.
 */
class Callback_Link {
    private:
        Callback* _callback;
        Callback_Link* next = NULL; // allow building chain
    public:
        Callback_Link(Callback& cb) {
            _callback = &cb;
            // cb.set_device(device); // executed in set_filter_callback
        }
        bool call() {
            bool result = _callback->call();
            if(result && next) { // traverse chain down to end
                result &= next->call();
            }
            return result;
        }
        bool chain(Callback& cb) {
            if(next) {
                return next->chain(cb); // traverse chain down to end
            } else {
                next = new Callback_Link(cb);
                return next != NULL;
            }
        }
};

/**
 * @class Subdevice
 * @brief Represents one value/channel of a Device
 * 
 * PURPOSE
 * =======
 * Many physical devices produce multiple values. For example:
 * - DHT sensor: temperature AND humidity
 * - RGB LED: red, green, blue channels
 * - GPS: latitude, longitude, altitude
 * 
 * Each of these values is a Subdevice with:
 * - Its own name (appended to device name for MQTT topic)
 * - Its own measured and confirmed values
 * - Its own subscription capability for receiving commands
 * - Its own receive callback
 * 
 * MQTT TOPIC STRUCTURE
 * ====================
 * If device name is "temp_sensor" and subdevice name is "celsius":
 * - Full topic: <node_topic>/temp_sensor/celsius
 * - Command topic: <node_topic>/temp_sensor/celsius/set
 * 
 * If subdevice has no name (empty string):
 * - Full topic: <node_topic>/temp_sensor
 * - Command topic: <node_topic>/temp_sensor/set
 * 
 * VALUE LIFECYCLE
 * ===============
 * - measured_value: Latest reading from sensor (may not be published yet)
 * - last_confirmed_value: Last value that was published to MQTT
 * - Values are compared to detect changes
 * - Only changed values are published (reduces MQTT traffic)
 * 
 * SUBSCRIPTION
 * ============
 * If subscribed = true:
 * - Device will subscribe to <topic>/set on MQTT
 * - Incoming messages trigger the receive_cb callback
 * - Callback can update device state based on command
 */
class Subdevice {
    private:
        Ustring name;  // Subdevice name (appended to device name for MQTT topic)
        bool _subscribed = false;  // Should this subdevice subscribe to commands?
        void init_log();
        void init(bool subscribed);
        void init(const char* subname, bool subscribed);
        void init(const __FlashStringHelper* subname, bool subscribed);
        #define IOTEMPOWER_ON_RECEIVE_CALLBACK std::function<bool(const Ustring&)>
        IOTEMPOWER_ON_RECEIVE_CALLBACK receive_cb = NULL;  // Called when MQTT command received
    public:
        Ustring measured_value;         // The just measured value (after calling measure())
        Ustring last_confirmed_value;   // Last value published to MQTT
        Ustring& get_last_confirmed_value() { return last_confirmed_value; }
        Ustring& get() { return measured_value; }
        Subdevice& with_receive_cb(IOTEMPOWER_ON_RECEIVE_CALLBACK cb) {
            receive_cb = cb;
            return *this;
        }
        bool call_receive_cb(Ustring& payload);
        const Ustring& get_name() const { return name; }
        const Ustring& key() const { return name; }
        bool subscribed() { return _subscribed; }
        Subdevice(bool subscribed) {
            init(subscribed);
        }
        Subdevice() { 
            init(false);
        }
        Subdevice(const char* subname, bool subscribed) {
            init(subname, subscribed);
        }
        Subdevice(const char* subname) { 
            init(subname, false);
        }
        Subdevice(const __FlashStringHelper* subname, bool subscribed) {
            init(subname, subscribed);
        }
        Subdevice(const __FlashStringHelper* subname) { 
            init(subname, false);
        }
};

/**
 * @class Device
 * @brief Base class for all IoTempower devices (sensors and actuators)
 * 
 * ARCHITECTURE OVERVIEW
 * =====================
 * This is the foundation of IoTempower's device system. All sensors and actuators
 * inherit from this class and gain automatic:
 * - Registration with DeviceManager
 * - MQTT topic generation
 * - Polling and measurement scheduling
 * - Change detection and publishing
 * - Command reception and routing
 * - Home Assistant integration
 * 
 * KEY FEATURES
 * ============
 * 
 * 1. MULTIPLE SUBDEVICES
 *    A single Device can have multiple subdevices for multi-value sensors.
 *    Example: DHT sensor has "temperature" and "humidity" subdevices.
 * 
 * 2. HOME ASSISTANT DISCOVERY
 *    If mqtt_discovery_prefix is defined, devices automatically:
 *    - Publish discovery messages to Home Assistant
 *    - Appear in Home Assistant without manual configuration
 *    - Include proper device metadata and capabilities
 * 
 * 3. FLEXIBLE POLLING
 *    Each device has its own poll rate (in microseconds):
 *    - Fast polling (1000us): For precise timing needs
 *    - Normal polling (100000us = 0.1s): For most sensors
 *    - Slow polling (1000000us = 1s): For slow-changing sensors
 * 
 * 4. CALLBACK SYSTEM
 *    Two types of callbacks:
 *    - Filter callbacks: Process measurements before acceptance
 *    - Change callbacks: Trigger when values change
 * 
 * 5. AUTOMATIC MQTT INTEGRATION
 *    No manual MQTT code needed:
 *    - Topics auto-generated from device and subdevice names
 *    - Publishing happens automatically when values change
 *    - Subscriptions set up automatically
 *    - Message routing handled by DeviceManager
 * 
 * DEVICE LIFECYCLE
 * ================
 * 1. Construction: Device object created, registers with DeviceManager
 * 2. start(): Initialize hardware (override this in derived classes)
 * 3. measure(): Read sensor/update actuator (called at pollrate)
 * 4. publish(): Send to MQTT (automatic when values change)
 * 5. receive(): Handle commands (automatic routing from MQTT)
 * 
 * CREATING A NEW DEVICE TYPE
 * ==========================
 * To create a new device type:
 * 1. Inherit from Device (or a specialized base like I2C_Device)
 * 2. Call Device constructor with name and pollrate
 * 3. Override start() to initialize hardware
 * 4. Override measure() to read sensor or update actuator
 * 5. Use value() to get/set subdevice values
 * 6. Optionally add callbacks for filtering or change detection
 */
class Device {
    protected:
        Fixed_Map<Subdevice, IOTEMPOWER_MAX_SUBDEVICES> subdevices;  // List of subdevices
#ifdef mqtt_discovery_prefix
        // Home Assistant MQTT Discovery support
        // TODO: might have to move this to subdevice (if there are two values measured)
        String discovery_config_topic;  // Topic for discovery config
        String discovery_info;          // JSON discovery payload
        void create_discovery_info(const String& type,
            bool state_topic=true, const char* state_on=NULL, const char* state_off=NULL,
            bool command_topic=false, const char* payload_on=NULL, const char* payload_off=NULL,
            const String& extra_json=String());
#endif
    private:
        // PRECISION CONTROL
        // Some devices (like LED strips) need very precise timing
        bool requires_precision = false;  // If true, only update in precision intervals
        
        // IDENTIFICATION
        Ustring name;  // Device name, used as MQTT topic component
        
        // PUBLISHING CONTROL
        bool _ignore_case = true;        // Convert values to lowercase before comparison
        bool _report_change = true;      // Should changes be published?
        bool _needs_publishing = false;  // Does this device have unpublished changes?
        bool _retained = false;          // Should MQTT messages be retained?

        /**
         * FILTER CALLBACK CHAIN
         * =====================
         * Called for every measurement to validate/modify the value.
         * 
         * The chain receives the device with measured_value filled in.
         * Each callback can:
         * - Modify measured_value
         * - Return true to accept the value
         * - Return false to reject the value
         * 
         * Common uses:
         * - Range validation (reject out-of-range values)
         * - Calibration (adjust readings)
         * - Noise filtering (average multiple readings)
         * - Unit conversion
         * 
         * Example:
         *   device.add_filter([](Device& dev) {
         *       float temp = dev.value().as_float();
         *       if (temp < -40 || temp > 85) return false;  // Out of range
         *       temp = temp * 1.02 - 0.5;  // Calibration
         *       dev.value().from(temp);
         *       return true;
         *   });
         */
        Callback_Link *filter_first;

        /**
         * CHANGE CALLBACK CHAIN
         * =====================
         * Called when a value changes (after filter chain accepts it).
         * 
         * The chain receives the device with new measured_value.
         * Each callback can:
         * - Read the old value (last_confirmed_value)
         * - Read the new value (measured_value)
         * - Perform actions based on the change
         * - Return true to accept the change
         * - Return false to reject the change
         * 
         * Common uses:
         * - Trigger actions (turn on light when motion detected)
         * - Send alerts (notify when temperature too high)
         * - Log changes
         * - Update related devices
         * 
         * Example:
         *   button.on_change([](Device& dev) {
         *       if (dev.value().equals("on")) {
         *           light.value().from("on");
         *       }
         *       return true;
         *   });
         */
        Callback_Link *on_change_first;

        // POLLING CONTROL
        unsigned long _pollrate_us = 0;    // Microseconds between polls (0 = as fast as possible)
        unsigned long last_poll = micros(); // Last time device was polled

    protected:
        bool _started = false;

    public:
        Device(const char* _name, unsigned long pollrate_us);
        //// Getters & Setters
        unsigned int subdevices_count() {
            return subdevices.length();
        }
        void demand_precision() { requires_precision = true; }
        bool needs_precision() { return requires_precision; }
        Device& with_ignore_case(bool ignore_case) { 
            _ignore_case = ignore_case;
            return *this;
        }
        Device& set_ignore_case(bool ignore_case) {
            return with_ignore_case(ignore_case);
        }
        Device& ignore_case(bool ignore_case) {
            return with_ignore_case(ignore_case);
        }

        bool get_ignore_case() {
            return _ignore_case;
        }


        ///////// Publishing
        bool needs_publishing() {
            return _needs_publishing;
        }

        // publish current value(s) and resets needs_publishing state
        ////AsyncMqttClient disabled in favor of PubSubClient
        //bool publish(AsyncMqttClient& mqtt_client, Ustring& node_topic);
        bool publish(PubSubClient& mqtt_client, Ustring& node_topic, Ustring& log_buffer);

#ifdef mqtt_discovery_prefix
        ////AsyncMqttClient disabled in favor of PubSubClient
        // bool publish_discovery_info(AsyncMqttClient& mqtt_client);
        bool publish_discovery_info(PubSubClient& mqtt_client);
#endif

        Ustring& get_last_confirmed_value(unsigned long index=0);
        Ustring& value(unsigned long index=0);
        // access value()
        double read_float(unsigned long index=0) { return value(index).as_float(); }
        long read_int(unsigned long index=0) { return value(index).as_int(); }
        void write_float(double v, unsigned long index=0) { value(index).from(v); }
        void write_int(long v, unsigned long index=0) { value(index).from(v); }
        void write(const char* s, unsigned long index=0) { value(index).from(s); }
        bool is(const char* s, unsigned long index=0) { return value(index).equals(s); }

        const Ustring& get_name() const { return name; }
        //const char* get_name() const { return name.as_cstr(); }
        const Ustring& key() const { return name; }
        
        virtual ~Device() {
            // usually nothing has to be done here
            // This virtual method needs to be defined to prevent warning
            // from cpp-compiler
        }


        //// pollrate
        Device& set_pollrate_us(unsigned long rate_us) { 
            _pollrate_us = rate_us;
            return *this;
        }
        Device& with_pollrate_us(unsigned long rate_us) { 
            return set_pollrate_us(rate_us);
        }
        Device& pollrate_us(unsigned long rate_us) { 
            return set_pollrate_us(rate_us);
        }
        Device& set_pollrate(unsigned long rate_ms) { 
            return set_pollrate_us(rate_ms*1000);
        }
        Device& with_pollrate(unsigned long rate_ms) { 
            return set_pollrate(rate_ms);
        }
        Device& pollrate(unsigned long rate_ms) { 
            return set_pollrate(rate_ms);
        }
        unsigned int get_pollrate() {
            return _pollrate_us/1000;
        }
        unsigned int get_pollrate_us() {
            return _pollrate_us;
        }

        //// report_change
        Device& set_report_change(bool report_change) { 
            _report_change = report_change;
            return *this;
        }
        Device& with_report_change(bool report_change) {
            return set_report_change(report_change);
        } 
        Device& report_change(bool report_change=true) {
            return set_report_change(report_change);
        } 
        bool get_report_change() { 
            return _report_change;
        }

        //// set if published mqtt messages should be retained
        Device& set_retained(bool retained) { 
            _retained = retained;
            return *this;
        }
        Device& with_retained(bool retained) {
            return set_retained(retained);
        } 
        Device& retained(bool retained=true) {
            return set_retained(retained);
        } 
        bool get_retained() {
            return _retained;
        }


        //// on_change_callback
        Device& set_on_change_callback(Callback& on_change_cb) {
            bool result;
            on_change_cb.set_device(*this);
            if(on_change_first) result=on_change_first->chain(on_change_cb);
            else {
                on_change_first = new Callback_Link(on_change_cb);
                result = on_change_first != NULL;
            }
            if(!result) {
                ulog(F("Trouble reserving on_change entry."));
            }
            return *this;
        }
        Device& with_on_change_callback(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        Device& on_change_callback(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        bool call_on_change_callbacks() {
            if(on_change_first) {
                return on_change_first->call();
            } else {
                return true; // all ok if there are no on_change callbacks
            }
        }

        // filter_callback
        Device& set_filter_callback(Callback& filter_cb) { 
            bool result;
            filter_cb.set_device(*this);
            if(filter_first) result=filter_first->chain(filter_cb);
            else {
                filter_first = new Callback_Link(filter_cb);
                result = filter_first != NULL;
            }
            if(!result) {
                ulog(F("Trouble reserving filter entry."));
            }
            return *this;
        }
        Device& with_filter_callback(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        Device& filter_callback(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        bool call_filter_callbacks() {
            if(filter_first) {
                return filter_first->call();
            } else {
                return true; // all ok if there are no filters
            }
        }

        // Subdevices
        Subdevice* add_subdevice(Subdevice* sd) {
            ulog(F("add_subdevice: device: %s subdev: >%s<"), name.as_cstr(), sd->get_name().as_cstr());
            if(subdevices.add(sd)) {
                ulog(F("add_subdevice >%s< succeeded."), sd->get_name().as_cstr());
                return sd;
            } else {
                ulog(F("add_subdevice >%s< failed."), sd->get_name().as_cstr());
                return NULL;
            }
        }

        Subdevice* subdevice(unsigned long index) {
            return subdevices.get(index);
        }

        bool subdevices_for_each(std::function<bool(Subdevice&)> func) {
            return subdevices.for_each(func);
        }

        bool started() { return _started; }

        /* poll_measure
         * This calls measure and filters and sets if necessary default values. 
         * This will be called very often from event loop
         * Will return if current measurement was successful or invalid. 
         * */
        bool poll_measure();

        /* check_changes
         * returns True if new values were measured and sets the publishing flag
         * This will be called very often from event loop 
         * This only reports change when the measured (and filtered) 
         * value is new (has changed) and report_changes is set.
         * If a change happened and a callback is set, the callback is called.
         * */
        bool check_changes();

        /* measure_init
         * Can be overloaded to allow re-initialization of some hardware
         * elements. For example a respective i2c bus due to a very weird
         * implementation of TwoWire in Wire.h
         * */
        virtual void measure_init() {}; // usually do nothing here

        /* measure_init
         * Can be overloaded to allow restoring of some hardware
         * elements. For example a respective i2c bus due to a very weird
         * implementation of TwoWire in Wire.h
         * */
        virtual void measure_exit() {}; // usually do nothing here

        /* start
         * This usually needs to be overwritten
         * Any physical initialization has to go here.
         * This is executed by the system right after all the rest has been 
         * intialized - right before the first subscription.
         * If something with the physical initialization goes wrong, started
         * should stay false. If physical intitialization worked this method
         * needs to set _started to true;
         * TODO: make protected?
         * */
        virtual void start() { _started = true; }

        /* measure
         * Does nothing by default.
         * Usually this needs to be overwritten.
         * It should make sure to trigger necessary steps to
         * measure from the physical hardware a sensor value (or
         * several values from a multi sensor).
         * It needs to set in measured_value the currently measured value.
         * If no value can be measured (or has not been), it should return
         * false. If measuring was successful, return True.
         * This is called from update to trigger the actual value generation.
         * If device is not marked as started, measure will not be called.
         * TODO: make protected?
         * */
        virtual bool measure() { return true; }

        /* buffer_reset
         * This usually needs to be overwritten
         * For input functions this allows to reset the precise buffer if used.
         * Does nothing by default.
         */
        virtual void buffer_reset() {};
};

// helpers for callbacks
#define on_change(f) with_on_change_callback(*new Callback(f))

#endif // _IOTEMPOWER_DEVICE_H_
