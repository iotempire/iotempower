/**
 * @file device-manager.h
 * @brief Central manager for all devices and the do_later scheduler
 * @author ulno
 * @created 2018-07-16
 * 
 * DEVICEMANAGER - THE HEART OF IOTEMPOWER
 * ========================================
 * The DeviceManager is a singleton that manages all devices on a node.
 * It's responsible for:
 * - Device registration and tracking
 * - Device lifecycle management (start, update, publish)
 * - Coordinating MQTT operations across all devices
 * - Managing the do_later scheduler
 * 
 * WHY DEVICEMANAGER IS ESSENTIAL
 * ==============================
 * In a standard Arduino project, you would need:
 * 
 *   Device* devices[10];
 *   int device_count = 0;
 *   
 *   void setup() {
 *     devices[device_count++] = &temp_sensor;
 *     devices[device_count++] = &humidity_sensor;
 *     devices[device_count++] = &led;
 *     // Initialize each device
 *     for(int i=0; i<device_count; i++) {
 *       devices[i]->start();
 *     }
 *   }
 *   
 *   void loop() {
 *     // Poll each device
 *     for(int i=0; i<device_count; i++) {
 *       devices[i]->poll();
 *     }
 *     // Publish each device
 *     for(int i=0; i<device_count; i++) {
 *       devices[i]->publish();
 *     }
 *   }
 * 
 * With IoTempower's DeviceManager:
 *   // Just create devices - that's it!
 *   dht11(temp_hum, D4);  // creates device for topic temp_hum with subdevices
 *   output(led, D5);
 *   // Everything else is automatic!
 * 
 * THE DO_LATER SCHEDULER
 * ======================
 * The do_later system provides delayed callback execution, useful for:
 * - Debouncing button presses
 * - Delayed actions (e.g., turn off after 5 minutes)
 * - Deep sleep scheduling
 * - Periodic tasks without blocking
 * 
 * Features:
 * - Sorted execution queue (earliest tasks execute first)
 * - Support for both ID-based (replaceable) and anonymous callbacks
 * - Automatic cleanup after execution
 * - Non-blocking (integrates with main loop)
 * 
 * Example:
 *   // Turn off LED after 5 seconds
 *   do_later(5000, [] {
 *     led.value().from("off");
 *   });
 * 
 * SINGLETON PATTERN
 * =================
 * DeviceManager uses the singleton pattern because:
 * 1. There should only be one device registry per node
 * 2. Devices need to register themselves during static initialization
 * 3. The main loop needs access to all devices
 * 4. MQTT callbacks need to route messages to devices
 * 
 * Access via: device_manager (a macro for DeviceManager::get_instance())
 */

#ifndef _IOTEMPOWER_DEVICE_MANAGER_H_
#define _IOTEMPOWER_DEVICE_MANAGER_H_

#include <espMqttClient.h>
#include <toolbox.h>
#include <device.h>

/**
 * DO_LATER SCHEDULER - NON-BLOCKING DELAYED EXECUTION
 * ====================================================
 * The do_later functions provide a simple way to schedule callbacks for future execution.
 * This is essential for IoT applications that need timing without blocking.
 * 
 * TWO VARIANTS
 * ============
 * 
 * 1. WITH ID (Replaceable callbacks):
 *    do_later(delay_ms, id, callback)
 *    - If a callback with the same id is already scheduled, it's replaced
 *    - Useful for debouncing or ensuring only one pending action
 *    - Example: Button press scheduling repeated callbacks - only latest matters
 * 
 * 2. WITHOUT ID (One-shot callbacks):
 *    do_later(delay_ms, callback)
 *    - Creates a new scheduled callback
 *    - Multiple callbacks can be scheduled simultaneously
 *    - Example: Turn on multiple devices at different times
 * 
 * COMPARISON TO STANDARD ARDUINO
 * ==============================
 * 
 * Standard Arduino (blocking):
 *   button_pressed = digitalRead(BUTTON_PIN);
 *   if (button_pressed) {
 *     delay(5000);  // BLOCKS EVERYTHING!
 *     led.off();
 *   }
 * 
 * Standard Arduino (non-blocking but complex):
 *   unsigned long button_time = 0;
 *   bool waiting = false;
 *   
 *   void loop() {
 *     if (digitalRead(BUTTON_PIN)) {
 *       button_time = millis();
 *       waiting = true;
 *     }
 *     if (waiting && millis() - button_time >= 5000) {
 *       led.off();
 *       waiting = false;
 *     }
 *   }
 * 
 * With IoTempower do_later:
 *   if (IN(button).is("pressed")) {
 *     do_later(5000, [] {
 *       IN(led).on();
 *     });
 *   }
 * 
 * IMPLEMENTATION DETAILS
 * ======================
 * - Callbacks stored in sorted array (by execution time)
 * - do_later_check() called in main loop to execute ready callbacks
 * - Handles time overflow (millis() rollover after ~49 days)
 * - Automatic cleanup after execution
 * - Maximum delay: 24 hours (IOTEMPOWER_MAX_DO_LATER_INTERVAL)
 * - Maximum scheduled callbacks: 64 (IOTEMPOWER_DO_LATER_MAP_SIZE)
 */

// Scheduler callback types
//typedef void (*DO_LATER_CB_ID)(int16_t);
typedef std::function<void(int16_t)> DO_LATER_CB_ID;
bool do_later(unsigned long in_ms, int16_t id, DO_LATER_CB_ID callback);
//typedef void (*DO_LATER_CB_NO_ID)();
typedef std::function<void()> DO_LATER_CB_NO_ID;
bool do_later(unsigned long in_ms, DO_LATER_CB_NO_ID callback);
void do_later_check(); // Internal - called by main loop, not by user code


/**
 * @class DeviceManager
 * @brief Singleton manager for all devices on the node
 * 
 * SINGLETON PATTERN
 * =================
 * DeviceManager is a singleton - only one instance exists per node.
 * Access it via the device_manager macro (defined at bottom of file).
 * 
 * DEVICE LIFECYCLE MANAGEMENT
 * ===========================
 * The DeviceManager orchestrates the entire device lifecycle:
 * 
 * 1. REGISTRATION (during static initialization)
 *    add(device) - Called by Device constructor
 * 
 * 2. INITIALIZATION (in setup())
 *    start() - Calls start() on all registered devices
 * 
 * 3. OPERATION (in loop())
 *    update(precision_interval) - Polls devices based on timing requirements
 *    publish() - Publishes changed values to MQTT
 * 
 * 4. COMMUNICATION
 *    subscribe() - Sets up MQTT subscriptions for all devices
 *    receive() - Routes incoming MQTT messages to correct device
 *    publish_discovery_info() - Publishes Home Assistant discovery
 * 
 * KEY METHODS
 * ===========
 */
class DeviceManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the single DeviceManager instance
     */
    static DeviceManager& get_instance();

    /**
     * @brief Register a device (called automatically by Device constructor)
     * @param device Reference to device to add
     * @return true if added successfully, false if device list is full
     */
    bool add(Device &device);
    
    /**
     * @brief Start all registered devices
     * Calls start() on each device to initialize hardware.
     * @return true if all devices started successfully
     */
    bool start();
    
    /**
     * @brief Update all devices (poll and measure)
     * 
     * Called from main loop. Manages two timing modes:
     * - Precision interval: Only precision devices update (tight timing)
     * - Regular interval: All devices update, yields allowed
     * 
     * @param in_precision_interval true for precision mode, false for regular
     * @return true if any device values changed
     */
    bool update(bool in_precision_interval);
    
    /**
     * @brief Reset measurement buffers for all devices
     * Called at start of precision intervals for devices that accumulate data.
     */
    void reset_buffers();
    
    /**
     * @brief Publish device values to MQTT
     * 
     * Iterates through all devices and publishes changed values.
     * 
     * @param mqtt_client espMqttClient instance
     * @param node_topic Base MQTT topic for this node
     * @param publish_all If true, publish all values; if false, only changed values
     * @return true if any values were published
     */
    bool publish(espMqttClient& mqtt_client, Ustring& node_topic, bool publish_all);
    
    /**
     * @brief Subscribe to MQTT command topics for all devices
     * @param mqtt_client espMqttClient instance
     * @param node_topic Base MQTT topic for this node
     * @return true if subscriptions successful
     */
    bool subscribe(espMqttClient& mqtt_client, Ustring& node_topic);
    
    /**
     * @brief Publish Home Assistant discovery messages for all devices
     * @param mqtt_client espMqttClient instance
     * @return true if discovery info published successfully
     */
    bool publish_discovery_info(espMqttClient& mqtt_client);
    
    /**
     * @brief Route incoming MQTT message to correct device
     * 
     * Called by MQTT callback when message received.
     * Parses subtopic to identify target device and calls its receive handler.
     * 
     * @param subtopic Topic after node_topic prefix (e.g., "led/set")
     * @param payload Message payload
     * @return true if message was handled
     */
    bool receive(Ustring& subtopic, Ustring& payload);
    
    /**
     * @brief Get list of all devices for reporting
     * Used for diagnostic and configuration purposes.
     * @param b Buffer to fill with device information
     */
    void get_report_list(Fixed_Buffer& b);
    
    /**
     * @brief Log number of registered devices (for debugging)
     */
    void log_length();

    /**
     * DESIGN DECISIONS
     * ================
     * 
     * Device deletion not supported:
     * For simplicity, we don't allow device removal after registration.
     * This is acceptable because device configuration is static - devices
     * are defined at compile time, not dynamically at runtime.
     * This decision was made during the transition from MicroPython to C++.
     */

private:
    /**
     * PRIVATE MEMBERS - SINGLETON IMPLEMENTATION
     * ==========================================
     */
    
    /** Private constructor - use get_instance() instead */
    DeviceManager();
    
    /** Private destructor */
    ~DeviceManager();

    /** 
     * Device registry
     * Fixed-size map holding pointers to all registered devices.
     * Max size defined by IOTEMPOWER_MAX_DEVICES (default 32).
     */
    Fixed_Map<Device, IOTEMPOWER_MAX_DEVICES> device_list;

    /** Prevent copying and assignment (singleton pattern) */
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
};

/**
 * CONVENIENT ACCESS MACRO
 * =======================
 * Use 'device_manager' instead of 'DeviceManager::get_instance()' everywhere.
 * This makes code more readable and consistent with IoTempower style.
 */
#define device_manager (DeviceManager::get_instance())

#endif // _IOTEMPOWER_DEVICE_MANAGER_H_
