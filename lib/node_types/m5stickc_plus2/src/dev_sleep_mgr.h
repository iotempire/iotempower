// dev_sleep_mgr.h
// Header File for sleep management device for M5StickC Plus2

#ifndef _SLEEP_MGR_H_
#define _SLEEP_MGR_H_

#include <iotempower.h>

/**
 * @class SleepManager
 * @brief M5StickC Plus2 specific sleep management device for IoTempower
 * 
 * PURPOSE
 * =======
 * The SleepManager device provides centralized sleep management capabilities for M5StickC Plus2 nodes:
 * - ESP32 deep sleep mode control
 * - M5StickC Plus2 specific power management via StickCP2.Power
 * - Scheduled power-down operations
 * - MQTT-controlled power state management
 * 
 * MQTT INTERFACE
 * ==============
 * Topics:
 * - <node>/sleep_mgr       → Reports current sleep state and operations
 * - <node>/sleep_mgr/battery      → Reports battery status (voltage,level,charging)
 * - <node>/sleep_mgr/set          ← Receives sleep commands
 * 
 * Commands:
 * - "sleep"                                      → Immediate indefinite deep sleep
 * - "sleep <milliseconds>"                       → Immediate timed deep sleep
 * - "sleep now" or "sleep 0"                     → Immediate indefinite deep sleep
 * - "sleep later <milliseconds>"                 → Scheduled indefinite deep sleep
 * - "sleep <duration> later <delay>"             → Scheduled timed deep sleep
 * - "shutdown"                                   → Power off the device (M5StickC Plus2 specific)
 * 
 * Examples:
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep 30000"        # Sleep 30s now
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep"              # Sleep indefinitely now
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "sleep 60000 later 5000"  # Sleep 60s in 5s
 * - mosquitto_pub -t "bedroom/sensor1/sleep_mgr/set" -m "shutdown"           # Power off device
 * 
 * Battery Status Format:
 * - Format: "voltage,level,charging" (e.g., "3.85,75,1")
 * - voltage: Battery voltage in volts (float with 2 decimal places)
 * - level: Battery level percentage (0-100)
 * - charging: 1 if charging, 0 if not charging
 * 
 * M5StickC Plus2 SPECIFIC FEATURES
 * ================================
 * - Uses StickCP2.Power for advanced power management
 * - Battery status monitoring via StickCP2.Power
 * - Supports proper shutdown via power management IC
 * - Maintains RTC functionality during sleep
 * - Optimized power consumption for battery operation
 * 
 * DEEP SLEEP BEHAVIOR
 * ===================
 * ESP32 Deep Sleep:
 * - Reduces power consumption to ~10μA
 * - All GPIO pins go to high-impedance state
 * - WiFi and CPU are powered down
 * - Only RTC memory and peripherals remain active
 * - Wake up via RTC timer, external interrupt, or power cycle
 * 
 * Wake Up Process:
 * - ESP32 restarts completely (like power-on reset)
 * - All variables are reset, devices re-initialize
 * - Node reconnects to WiFi and MQTT
 * - Sleep duration is tracked via RTC memory if needed
 */
class SleepManager : public Device {
    private:
        bool _sleep_pending = false;          // Is a sleep operation pending?
        bool _shutdown_pending = false;       // Is a shutdown operation pending?
        unsigned long _sleep_duration_ms = 0; // Duration for pending sleep (0 = indefinite)
        unsigned long _sleep_scheduled_time = 0; // When sleep was scheduled (for countdown)
        unsigned long _sleep_delay_ms = 0;    // Delay before sleep starts
        const unsigned long MIN_SLEEP_DURATION_MS = 500; // Minimum 500ms sleep
        
        void enter_deep_sleep(unsigned long duration_ms = 0); // 0 = indefinite sleep
        void enter_shutdown(); // M5StickC Plus2 specific shutdown
        bool parse_sleep_command(const Ustring& payload, unsigned long& duration_ms, unsigned long& delay_ms, bool& shutdown);
        
    public:
        /**
         * @brief Construct a new SleepManager for M5StickC Plus2
         * @param name Device name for MQTT topics
         */
        SleepManager(const char* name);
        
        /**
         * @brief Initialize the sleep manager device with M5StickC Plus2 support
         * 
         * Sets up MQTT subscriptions and initial state reporting
         */
        void start() override;
        
        /**
         * @brief Regular device update with M5StickC Plus2 power management (called by DeviceManager)
         * 
         * Updates countdown status for pending sleep operations and manages M5StickC Plus2 power state
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
         * @brief Power off the M5StickC Plus2 device immediately
         * @return true if shutdown was initiated successfully
         */
        bool shutdown();
        
        /**
         * @brief Schedule shutdown after a delay
         * @param delay_ms Delay before shutdown starts in milliseconds
         * @return true if shutdown was scheduled successfully
         */
        bool shutdown_in(unsigned long delay_ms);
        
        /**
         * @brief Get current sleep management status
         * @return Status string for MQTT publishing
         */
        Ustring get_sleep_status();
};

/**
 * @brief Schedule device to enter deep sleep mode
 * 
 * Deep sleep is a power-saving mode where the ESP32 goes into hibernation
 * and wakes up after a specified duration or when reset.
 * 
 * @param time_from_now_ms Delay before entering deep sleep (in milliseconds)
 * @param duration_ms How long to sleep (0 = wake only via RST line)
 */
#define deep_sleep(time_from_now_ms, duration_ms) IN(sleep_mgr).sleep_in(time_from_now_ms, duration_ms)


#endif // _SLEEP_MGR_H_