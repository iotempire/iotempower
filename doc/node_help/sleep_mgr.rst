sleep_mgr
=========

**Automatically Available on All ESP Devices**

..  code-block:: cpp

    // No explicit declaration needed - sleep_mgr is automatically available
    // Optional: create additional instances with custom names
    sleep_mgr(name);

**other names:** ``sleep_manager``, ``power_mgmt``

The sleep management device is **automatically included** on all ESP-based IoTempower 
nodes without requiring explicit declaration. It provides centralized sleep control 
capabilities with a global instance named "sleep_mgr" that's immediately available for use.

Create additional sleep management devices for specialized use cases. The automatic
global instance supports ESP deep sleep mode with MQTT command interface for remote 
sleep management.

The sleep management device offers significant advantages over the legacy deep_sleep function:
automatic availability, remote control via MQTT, status reporting, Home Assistant integration, and better
integration with other devices.

Automatic Availability
---------------------

The sleep manager is automatically available on all ESP devices with the name "sleep_mgr". 
You can immediately use it via MQTT without any setup:

.. code-block:: bash

    # Send sleep commands to the automatic instance
    mosquitto_pub -t "node1/sleep_mgr/set" -m "sleep 30s"

Parameters
----------

- name: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

MQTT Interface
--------------

The sleep management device creates the following MQTT topics:

- ``<node>/name/status`` → Published sleep state and operations
- ``<node>/name/set`` ← Receives sleep commands

For the automatic global instance:

- ``<node>/sleep_mgr/status`` → Published sleep state and operations
- ``<node>/sleep_mgr/set`` ← Receives sleep commands

Commands
~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Command
     - Description
     - Example
   * - ``sleep``
     - Immediate indefinite deep sleep
     - ``sleep``
   * - ``sleep <ms>``
     - Timed deep sleep in milliseconds
     - ``sleep 30000``
   * - ``sleep now``
     - Immediate indefinite deep sleep
     - ``sleep now``
   * - ``sleep 0``
     - Immediate indefinite deep sleep
     - ``sleep 0``
   * - ``sleep later <ms>``
     - Scheduled indefinite deep sleep
     - ``sleep later 5000``
   * - ``sleep <duration> later <delay>``
     - Scheduled timed deep sleep
     - ``sleep 60000 later 5000``

Status Values
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Status
     - Description
   * - ``ready``
     - Device awake and ready for commands
   * - ``sleeping_in_<s>s_for_<ms>ms``
     - Countdown to scheduled timed sleep
   * - ``powering_off_in_<s>s``
     - Countdown to indefinite sleep
   * - ``sleeping_for_<ms>ms``
     - About to enter timed sleep
   * - ``powering_off``
     - About to enter indefinite sleep
   * - ``invalid_command``
     - Last command was invalid
   * - ``error_duration_too_short``
     - Sleep duration below minimum (500ms)

Deep Sleep Behavior
-------------------

When the device enters deep sleep mode:

- **Power Consumption**: ~20μA (ESP8266) or ~10μA (ESP32)
- **GPIO State**: All pins go high-impedance
- **WiFi/CPU**: Completely powered down
- **Memory**: All RAM is lost, device restarts on wake
- **RTC**: Only RTC timer and memory remain active

Wake Up Conditions
~~~~~~~~~~~~~~~~~~

1. **Timer Wake**: Automatic wake after specified duration
2. **Reset Wake**: Manual reset button press
3. **Power Cycle**: Power disconnection and reconnection

Example
-------

**node name:** ``bedroom/sensor1``

..  code-block:: cpp

    // sleep_mgr(sleep_mgr); this is done automatically for all ESP platforms
    dht22(temperature, D4);
    led(status, D2);

Now you can control sleep management via MQTT:

..  code-block:: bash

    # Sleep for 30 seconds (immediate)
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep 30000"
    
    # Sleep indefinitely (immediate)
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep"
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep now"
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep 0"
    
    # Schedule sleep for 1 minute, starting in 5 seconds
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep 60000 later 5000"
    
    # Schedule indefinite sleep in 10 seconds
    mosquitto_pub -t "bedroom/sensor1/sleep_control/set" -m "sleep later 10000"
    
    # Monitor status
    mosquitto_sub -t "bedroom/sensor1/sleep_control/status"

Command Examples
~~~~~~~~~~~~~~~~

Here are all the supported command variations:

.. list-table::
   :header-rows: 1

   * - Command
     - Action
   * - ``sleep``
     - Sleep indefinitely right now
   * - ``sleep now``
     - Sleep indefinitely right now
   * - ``sleep 0``
     - Sleep indefinitely right now
   * - ``sleep 30000``
     - Sleep for 30 seconds right now
   * - ``sleep later 5000``
     - Sleep indefinitely in 5 seconds
   * - ``sleep now later 5000``
     - Sleep indefinitely in 5 seconds
   * - ``sleep 30000 later 5000``
     - Sleep for 30 seconds, starting in 5 seconds

Integration with Other Devices
------------------------------

The sleep management device can be integrated with other IoTempower devices for automated
sleep management. The automatic global instance is immediately available:

..  code-block:: cpp

    // No need to declare sleep_mgr - it's automatically available
    button(sleep_button, D1).with_on_change([&] (Device& device) {
            if (sleep_button.get_last_confirmed_value().equals("on")) {
                IN(sleep_mgr).sleep(60000); // Sleep for 60 seconds
            }
            return true;
        });
    dht22(temperature, D4);


Accessing the Automatic Instance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The automatic sleep manager can be accessed in several ways:

..  code-block:: cpp

    // Via the global variable name
    IN(sleep_mgr).sleep(30000);
    
    // Via MQTT (external control)
    // mosquitto_pub -t "node1/sleep_mgr/set" -m "sleep 30s"

Battery Life Optimization
-------------------------

With proper sleep management, battery life can be extended significantly:

.. list-table::
   :header-rows: 1

   * - Sleep Duration
     - Wake Time
     - Battery Life (2000mAh)
   * - 10 minutes
     - 5 seconds
     - ~3-6 months
   * - 1 hour
     - 5 seconds
     - ~1-2 years
   * - 12 hours
     - 5 seconds
     - ~5+ years

Battery-Optimized Example
~~~~~~~~~~~~~~~~~~~~~~~~~

..  code-block:: cpp

    sleep_mgr(sleep_control);
    dht22(temperature, D4);

    void start() {
      // Sleep in 5 seconds to allow publishing
      IN.sleep_in(5000, 10 * 60 * 1000); // 10 minutes
    }

This pattern achieves:
- Wake up and connect to WiFi (~6 seconds)
- Take sensor reading (~0.1 seconds)
- Publish to MQTT (~0.5 seconds)
- Enter deep sleep for 10 minutes (~20μA power consumption)

Home Assistant Integration
--------------------------

The sleep management device automatically registers with Home Assistant via MQTT discovery
as a switch device. You can create automations:

..  code-block:: yaml

    # Sleep device at night
    automation:
      - alias: "Sleep bedroom sensor at night"
        trigger:
          platform: time
          at: "23:00:00"
        action:
          service: mqtt.publish
          data:
            topic: "bedroom/sensor1/sleep_mgr/set"
            payload: "sleep 28800000"  # 8 hours

Safety Features
---------------

- **Minimum Sleep Duration**: 1 second minimum to prevent rapid cycling
- **Command Validation**: Invalid commands are rejected with error status
- **Graceful Shutdown**: Status is published before entering sleep
- **Platform Detection**: Automatic ESP8266/ESP32 compatibility

Troubleshooting
---------------

Common Issues
~~~~~~~~~~~~~

1. **Device doesn't wake up**: Check power supply and reset button
2. **Sleep command ignored**: Verify MQTT connection and topic
3. **Rapid wake/sleep cycles**: Check minimum sleep duration (1000ms)
4. **Status not published**: Ensure MQTT broker is accessible

Methods
-------

The sleep management device provides these methods for programmatic control:

- ``sleep(duration_ms)`` - Sleep immediately (0 = indefinite)
- ``sleep_in(delay_ms, duration_ms)`` - Schedule sleep after delay
- ``get_sleep_status()`` - Get current sleep state

Programming Examples
~~~~~~~~~~~~~~~~~~~~

The automatic sleep manager instance is immediately available for use:

..  code-block:: cpp

    void start() {
        // Sleep immediately for 5 minutes using automatic instance
        IN(sleep_mgr).sleep_now(5 * 60 * 1000);
        
        // Schedule sleep in 30 seconds for 10 minutes
        IN(sleep_mgr).sleep_in(30000, 10 * 60 * 1000);
        
        // Power off immediately (indefinite sleep)
        IN(sleep_mgr).sleep_now(0);
        
        // Schedule power off in 1 minute
        IN(sleep_mgr).sleep_in(60000, 0);
    }

**Simple Sleep Example** (no setup.cpp code needed):

..  code-block:: bash

    # Just send MQTT commands to any ESP node
    mosquitto_pub -t "bedroom_sensor/sleep_mgr/set" -m "sleep 1h"
    mosquitto_pub -t "garden_monitor/sleep_mgr/set" -m "sleep 30s"

Legacy deep_sleep Function
---------------------------

.. note::
   The legacy ``deep_sleep(time_from_now_ms, duration_ms)`` function is still
   available but the sleep management device is recommended for new projects as it provides
   better integration, remote control, and status monitoring.

..  code-block:: cpp

    void start() {
        deep_sleep(10000, 30000);  // Sleep in 10s for 30s
    }

For the legacy function, remember to wire ``RST`` to ``D0`` (pin 16) for wake-up.

Future Extensions
-----------------

The sleep management device is designed to be extended for:

- **M5Stack Power Management**: Support for M5Stack's advanced power ICs
- **Battery Monitoring**: Integration with battery level sensors
- **Solar Power**: Coordination with solar charging systems
- **Light Sleep Mode**: ESP32 light sleep for partial power savings
- **Wake on Interrupt**: External pin wake-up capabilities

Platform Compatibility
-----------------------

.. list-table::
   :header-rows: 1

   * - Platform
     - Deep Sleep
     - Indefinite Sleep
     - Notes
   * - ESP8266
     - ✅
     - ✅
     - Native support
   * - ESP32
     - ✅
     - ✅
     - Uses max timer value
   * - Others
     - ⚠️
     - ⚠️
     - Fallback delay()