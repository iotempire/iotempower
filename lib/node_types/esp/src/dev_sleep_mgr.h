// dev_sleep_mgr.h
// Header File for sleep management device (deep sleep, power control)

#ifndef _SLEEP_MGR_H_
#define _SLEEP_MGR_H_

#include <device.h>

/**
 * @class SleepManager
 * @brief Generic sleep management device for IoTempower
 * 
 * PURPOSE
 * =======
 * The SleepManager device provides centralized sleep management capabilities for IoT nodes:
 * - ESP deep sleep mode control
 * - Scheduled power-down operations
 * - Future support for M5Stack power management controllers
 * - MQTT-controlled power state management
 * 
 * MQTT INTERFACE
 * ==============
 * Topics:
 * - <node>/sleep_mgr/status       → Reports current sleep state and operations
 * - <node>/sleep_mgr/set          ← Receives sleep commands
 * 
 * Commands:
 * - "sleep"                                      → Immediate indefinite deep sleep
 * - "sleep <milliseconds>"                       → Immediate timed deep sleep
 * - "sleep now" or "sleep 0"                     → Immediate indefinite deep sleep
 * - "sleep later <milliseconds>"                 → Scheduled indefinite deep sleep
 * - "sleep <duration> later <delay>"             → Scheduled timed deep sleep
 * 
 * Examples:
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep 30000"        # Sleep 30s now
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep"              # Sleep indefinitely now
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep 60000 later 5000"  # Sleep 60s in 5s
 * 
 * DEEP SLEEP BEHAVIOR
 * ===================
 * ESP8266/ESP32 Deep Sleep:
 * - Reduces power consumption to ~20μA (ESP8266) or ~10μA (ESP32)
 * - All GPIO pins go to high-impedance state
 * - WiFi and CPU are powered down
 * - Only RTC memory and peripherals remain active
 * - Wake up via RTC timer, external interrupt, or power cycle
 * 
 * Wake Up Process:
 * - ESP restarts completely (like power-on reset)
 * - All variables are reset, devices re-initialize
 * - Node reconnects to WiFi and MQTT
 * - Sleep duration is tracked via RTC memory if needed
 * 
 * FUTURE EXTENSIONS
 * =================
 * This device is designed to be extended for other power management scenarios:
 * - M5Stack power management ICs
 * - External power switching circuits
 * - Battery monitoring integration
 * - Solar panel charge controllers
 * 
 * SAFETY CONSIDERATIONS
 * =====================
 * - Always publish status before entering sleep mode
 * - Provide reasonable minimum sleep durations
 * - Include wake-up validation and reporting
 * - Handle invalid command gracefully
 */
class SleepManager : public Device {
    private:
        bool _sleep_pending = false;          // Is a sleep operation pending?
        unsigned long _sleep_duration_ms = 0; // Duration for pending sleep (0 = indefinite)
        unsigned long _sleep_scheduled_time = 0; // When sleep was scheduled (for countdown)
        unsigned long _sleep_delay_ms = 0;    // Delay before sleep starts
        const unsigned long MIN_SLEEP_DURATION_MS = 500; // Minimum 500ms sleep
        
        void enter_deep_sleep(unsigned long duration_ms = 0); // 0 = indefinite sleep
        bool parse_sleep_command(const Ustring& payload, unsigned long& duration_ms, unsigned long& delay_ms);
        
    public:
        /**
         * @brief Construct a new SleepManager
         * @param name Device name for MQTT topics
         */
        SleepManager(const char* name);
        
        /**
         * @brief Initialize the sleep manager device
         * 
         * Sets up MQTT subscriptions and initial state reporting
         */
        void start() override;
        
        /**
         * @brief Regular device update (called by DeviceManager)
         * 
         * Updates countdown status for pending sleep operations
         */
        bool measure() override;
        
        /**
         * @brief Sleep immediately for specified duration
         * @param duration_ms Sleep duration in milliseconds (0 = indefinite)
         * @return true if sleep was initiated successfully
         */
        bool sleep(unsigned long duration_ms = 0);
        
        /**
         * @brief Schedule sleep after a delay
         * @param delay_ms Delay before sleep starts in milliseconds
         * @param duration_ms Sleep duration in milliseconds (0 = indefinite)
         * @return true if sleep was scheduled successfully
         */
        bool sleep_in(unsigned long delay_ms, unsigned long duration_ms = 0);
        
        /**
         * @brief Get current sleep management status
         * @return Status string for MQTT publishing
         */
        Ustring get_sleep_status();
};

#endif // _SLEEP_MGR_H_