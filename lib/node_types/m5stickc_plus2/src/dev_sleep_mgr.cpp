// dev_sleep_mgr.cpp
// Implementation of sleep management device for M5StickC Plus2

#include "dev_sleep_mgr.h"
#include "device-manager.h"
#include "platform_includes.h"

// Include ESP32-specific power management
#include <esp_sleep.h>

SleepManager::SleepManager(const char* name) : Device(name, 1000000) { // Poll every 1 second for status updates
    // Add main status subdevice (publishes power state)
    add_subdevice(new Subdevice());
    
    // Add command subdevice that subscribes to power commands
    add_subdevice(new Subdevice(str_set, true))->with_receive_cb(
        [&] (const Ustring& payload) {
            ulog(F("Sleep command received: %s"), payload.as_cstr());
            
            unsigned long duration_ms = 0; // 0 = indefinite by default
            unsigned long delay_ms = 0;    // 0 = immediate by default
            bool shutdown = false;         // shutdown command flag
            
            
            // Parse the sleep command with all its variations
            if (parse_sleep_command(payload, duration_ms, delay_ms, shutdown)) {
                if (shutdown) {
                    // M5StickC Plus2 shutdown
                    if (delay_ms == 0) {
                        ulog(F("Sleep: Shutting down M5StickC Plus2"));
                        return this->shutdown();
                    } else {
                        ulog(F("Sleep: Scheduling shutdown in %lu ms"), delay_ms);
                        return shutdown_in(delay_ms);
                    }
                } else if (delay_ms == 0) {
                    // Immediate sleep
                    if (duration_ms == 0) {
                        ulog(F("Sleep: Entering indefinite deep sleep"));
                    } else {
                        ulog(F("Sleep: Entering timed deep sleep for %lu ms"), duration_ms);
                    }
                    return sleep(duration_ms);
                } else {
                    // Scheduled sleep
                    if (duration_ms == 0) {
                        ulog(F("Sleep: Scheduling indefinite deep sleep in %lu ms"), delay_ms);
                    } else {
                        ulog(F("Sleep: Scheduling timed deep sleep for %lu ms in %lu ms"), duration_ms, delay_ms);
                    }
                    return sleep_in(delay_ms, duration_ms);
                }
            }
            
            // Invalid command
            ulog(F("Sleep: Invalid command '%s'. Use 'sleep [<duration>] [later <delay>]' or 'shutdown [later <delay>]'"), payload.as_cstr());
            value().from(F("invalid_command"));
            return false;
        }
    );
    
    // Initialize with ready state
    value().from(F("ready"));
}

void SleepManager::start() {
    #ifdef mqtt_discovery_prefix
        // Create Home Assistant discovery info
        // Register as a switch that can be used to trigger sleep
        create_discovery_info(F("switch"), 
            true,   // command_topic (has set)
            NULL,   // no specific on command
            NULL,   // no specific off command  
            true,   // state_topic (publishes status)
            "ready", // on_state
            "off");  // off_state
    #endif
    
    _started = true;
    
    // Report that device is ready
    value().from(F("ready"));
    ulog(F("M5StickC Plus2 Sleep management device started"));
}

bool SleepManager::measure() {
    // Update M5StickC Plus2 power management
    StickCP2.update();
    
    // Update status to show countdown if sleep is pending
    if (_sleep_pending) {
        unsigned long current_time = millis();
        unsigned long elapsed = current_time - _sleep_scheduled_time;
        
        if (elapsed < _sleep_delay_ms) {
            // Still counting down
            unsigned long remaining = _sleep_delay_ms - elapsed;
            value().copy(get_sleep_status());
        } else {
            // Time to sleep/shutdown - this shouldn't happen as do_later should handle it
            // But include as failsafe
            _sleep_pending = false;
            if (_shutdown_pending) {
                _shutdown_pending = false;
                enter_shutdown();
            } else {
                enter_deep_sleep(_sleep_duration_ms);
            }
        }
    } else {
        // Update status normally
        value().copy(get_sleep_status());
    }
    return true;
}

bool SleepManager::sleep(unsigned long duration_ms) {
    // Validate minimum sleep duration (except for indefinite sleep)
    if (duration_ms > 0 && duration_ms < MIN_SLEEP_DURATION_MS) {
        ulog(F("Sleep: Sleep duration %lu ms too short, minimum is %lu ms"), 
             duration_ms, MIN_SLEEP_DURATION_MS);
        value().from(F("error_duration_too_short"));
        return false;
    }
    
    // Set status to indicate immediate sleep
    if (duration_ms == 0) {
        value().from(F("powering_off"));
    } else {
        Ustring status(F("sleeping_for_"));
        status.add(duration_ms);
        status.add(F("ms"));
        value().copy(status);
    }
    
    // Give a brief moment for status to be published
    do_later(100, [this, duration_ms]() {
        enter_deep_sleep(duration_ms);
    });
    
    return true;
}

bool SleepManager::sleep_in(unsigned long delay_ms, unsigned long duration_ms) {
    // Validate minimum sleep duration (except for indefinite sleep)
    if (duration_ms > 0 && duration_ms < MIN_SLEEP_DURATION_MS) {
        ulog(F("Sleep: Sleep duration %lu ms too short, minimum is %lu ms"), 
             duration_ms, MIN_SLEEP_DURATION_MS);
        return false;
    }
    
    // Set up pending sleep state
    _sleep_pending = true;
    _shutdown_pending = false;
    _sleep_duration_ms = duration_ms;
    _sleep_delay_ms = delay_ms;
    _sleep_scheduled_time = millis();
    
    // Schedule the actual sleep
    do_later(delay_ms, [this, duration_ms]() {
        _sleep_pending = false;
        enter_deep_sleep(duration_ms);
    });
    
    ulog(F("Sleep: Sleep scheduled in %lu ms for %lu ms"), delay_ms, duration_ms);
    return true;
}

bool SleepManager::shutdown() {
    value().from(F("shutting_down"));
    
    // Give a brief moment for status to be published
    do_later(100, [this]() {
        enter_shutdown();
    });
    
    return true;
}

bool SleepManager::shutdown_in(unsigned long delay_ms) {
    // Set up pending shutdown state
    _sleep_pending = true;
    _shutdown_pending = true;
    _sleep_duration_ms = 0; // Not used for shutdown
    _sleep_delay_ms = delay_ms;
    _sleep_scheduled_time = millis();
    
    // Schedule the actual shutdown
    do_later(delay_ms, [this]() {
        _sleep_pending = false;
        _shutdown_pending = false;
        enter_shutdown();
    });
    
    ulog(F("Sleep: Shutdown scheduled in %lu ms"), delay_ms);
    return true;
}

void SleepManager::enter_deep_sleep(unsigned long duration_ms) {
    if (duration_ms == 0) {
        ulog(F("Sleep: M5StickC Plus2 entering indefinite deep sleep"));
        
        // M5StickC Plus2 doesn't support indefinite sleep, use maximum value (~136 years)
        esp_sleep_enable_timer_wakeup(0x7FFFFFFFFFFFFFFFLL);
        
        // Use M5StickC Plus2 specific sleep preparation
        StickCP2.Power.deepSleep();
    } else {
        ulog(F("Sleep: M5StickC Plus2 entering deep sleep for %lu ms"), duration_ms);
        
        // ESP32 timed deep sleep with M5StickC Plus2 power management
        esp_sleep_enable_timer_wakeup(duration_ms * 1000); // expects microseconds
        
        // Use M5StickC Plus2 specific sleep preparation
        StickCP2.Power.timerSleep(duration_ms/1000); // expects seconds
    }
}

void SleepManager::enter_shutdown() {
    ulog(F("Sleep: M5StickC Plus2 shutting down"));
    
    // Use M5StickC Plus2 specific shutdown
    StickCP2.Power.powerOff();
}

bool SleepManager::parse_sleep_command(const Ustring& payload, unsigned long& duration_ms, unsigned long& delay_ms, bool& shutdown) {
    Ustring command(payload); // Make a mutable copy of the command

    // Check for shutdown command first
    shutdown = false;
    if (command.starts_with(F("shutdown"))) {
        shutdown = true;
    }
    if (!shutdown and !command.starts_with(F("sleep"))) {
        return false;
    }
    
    // Strip the "shutdown/sleep" command to get parameters
    command.strip_param(); // This removes "sleep" and the following space

    // Initialize defaults
    duration_ms = 0; // 0 = indefinite
    delay_ms = 0;    // 0 = immediate
    
    // If no parameters, indefinite sleep immediately
    if (command.empty()) {
        return true;
    }

    // Format: "sleep [<duration>] later <delay>"
    if (!command.starts_with(F("later"))) { // do we have a duration?
        if(command.starts_with(F("now"))) {
            duration_ms = 0; // indefinite
        } else {
            long duration_long = command.as_int();
            if (duration_long >= 0) {
                duration_ms = (unsigned long)duration_long;
            } else {
                return false; // Invalid duration
            }
        }
        command.strip_param(); // Remove duration part
    }

    if (command.starts_with(F("later"))) { // is there a later part?
        command.strip_param(); // Remove "later"
        if (command.empty()) {
            return false; // "later" without valid delay
        }        
        long delay_long = command.as_int();
        if (delay_long >= 0) {
            delay_ms = (unsigned long)delay_long;
            return true;
        } else {
            return false; // Invalid delay
        }
    }
    
    return true; // Valid command without "later"
}

Ustring SleepManager::get_sleep_status() {
    if (_sleep_pending) {
        unsigned long current_time = millis();
        unsigned long elapsed = current_time - _sleep_scheduled_time;
        
        if (elapsed < _sleep_delay_ms) {
            // Show countdown
            unsigned long remaining_delay = _sleep_delay_ms - elapsed;
            unsigned long remaining_seconds = (remaining_delay + 999) / 1000; // Round up
            
            Ustring status;
            if (_shutdown_pending) {
                status.from(F("shutting_down_in_"));
                status.add(remaining_seconds);
                status.add(F("s"));
            } else if (_sleep_duration_ms == 0) {
                status.from(F("powering_off_in_"));
                status.add(remaining_seconds);
                status.add(F("s"));
            } else {
                status.from(F("sleeping_in_"));
                status.add(remaining_seconds);
                status.add(F("s_for_"));
                status.add(_sleep_duration_ms);
                status.add(F("ms"));
            }
            return status;
        }
    }
    return Ustring(F("ready"));
}