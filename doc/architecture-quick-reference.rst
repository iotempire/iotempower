Architecture Quick Reference
============================

This is a quick reference guide to IoTempower's architecture and key concepts.

For detailed information, see:

- `Architecture Overview <architecture.rst>`_ - Complete system architecture
- `Device Architecture <device-architecture.rst>`_ - Device framework details
- `Deployment Process <deployment-process.rst>`_ - Build and deploy workflow


Key Concepts
------------

Systems, Nodes, and Devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block::

   IoT System (e.g., "my-home")
   └── Room/Collection (e.g., "bedroom") - folder with multiple nodes
       └── Node (e.g., "reading-lamp-left") - microcontroller
           └── Devices (e.g., "relay", "button", "status_led")

**System**: A complete IoT installation (one WiFi network, one MQTT broker)

**Room/Collection**: A folder containing multiple related nodes (e.g., bedroom with reading-lamp-left, reading-lamp-right, bed-leds, overhead-light, environment-reader)

**Node**: A microcontroller (ESP8266/ESP32) with connected devices, stored in a folder with node.conf and setup.cpp

**Device**: A sensor or actuator (temperature sensor, LED, button, relay, etc.)


Device Class Hierarchy
~~~~~~~~~~~~~~~~~~~~~~

.. code-block::

   Device (base class)
   ├── Input_Base → Input, Analog
   ├── Output (digital output, LED, relay)
   ├── PWM (pulse width modulation)
   ├── I2C_Device → BMP180, BH1750, etc.
   ├── Dht11, Dht22, Ds18b20 (OneWire devices)
   ├── Hcsr04 (acoustic distance sensor)
   └── RGB_Base → RGB_Single, RGB_Strip_Bus

Each level adds functionality:

1. **Device**: Core lifecycle, MQTT, polling
2. **Input_Base**: Pin management, buffers
3. **Input**: Digital reading, debouncing


Configuration Files
-------------------

system.conf (System Level)
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # WiFi and network
   IOTEMPOWER_AP_NAME="my-iot-network"
   IOTEMPOWER_AP_PASSWORD="secret123"
   
   # MQTT broker
   MQTT_HOST="192.168.1.100"
   MQTT_USER="mqtt"
   MQTT_PASSWORD="mqttpass"

node.conf (Node Level)
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Board type - any board listed in lib/node_types/ can be used
   board="wemos_d1_mini"
   # or: board="esp32dev", "nodemcu", "esp32", "esp32minikit", 
   #     "m5stickc", "m5stickcplus", "sonoff", "lolin_s2_mini"

setup.cpp (Device Level)
~~~~~~~~~~~~~~~~~~~~~~~~~

Located in: my-home/living-room/door-climate/setup.cpp

.. code-block:: cpp

   // Define your devices (note: name is not quoted)
   led(status, D1);
   button(door, D2);  // with_pull_up is default
   dht22(climate, D4);
   
   // Add interactions using IN() macro to access device reference
   IN(door).with_on_change_callback(*new Callback([](Device& dev) {
       if (dev.is("on")) {
           IN(status).on();
       }
       return true;
   }));


Device Lifecycle
----------------

Each device goes through these stages:

1. **Construction**
   
   .. code-block:: cpp
   
      Device(const char* name, unsigned long pollrate_us)

2. **Registration**
   
   Automatically added to DeviceManager

3. **Start**
   
   .. code-block:: cpp
   
      virtual void start() { _started = true; }
   
   Initialize hardware (set pin modes, initialize sensors)

4. **Measure**
   
   .. code-block:: cpp
   
      virtual bool measure() { return true; }
   
   Read sensor values (called at pollrate intervals)

5. **Publish**
   
   Changed values automatically published via MQTT (can be overridden in on_change 
   closure by returning false to prevent value registration)

6. **Receive**
   
   Commands received from MQTT subscriptions


MQTT Topics
-----------

Topic Structure
~~~~~~~~~~~~~~~

.. code-block::

   <node>/[<room>/[<subroom>/]]<device>/[subdevice]/[command or subsensor value]

Note: System name is not used in MQTT topics. Topics start at node level (often a room name).

Publishing (Sensor → MQTT)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Examples from bedroom with multiple nodes:

.. code-block::

   bedroom/reading-lamp-left/relay             → on
   bedroom/environment-reader/climate/temperature → 23.5
   bedroom/environment-reader/climate/humidity    → 45
   bedroom/bed-leds/strip                      → #ff0000

Subscribing (MQTT → Actuator)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block::

   bedroom/reading-lamp-left/relay/set         ← on
   bedroom/bed-leds/strip/set                  ← blue
   bedroom/overhead-light/dimmer/set           ← 75


Common Commands
---------------

Note: Most of these commands can also be triggered via ``iot menu`` in a TUI way.

System Management
~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Create new system
   create_system_template my-home
   
   # Create new node (or use mkdir for a room folder)
   cd my-home
   create_node_template living-room

Node Development
~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Edit device configuration (use micro, mcedit, or graphical editor)
   cd living-room
   micro setup.cpp
   # or: mcedit setup.cpp
   # or: code setup.cpp (VS Code)
   
   # Compile firmware (for testing setup.cpp without deploying)
   compile
   
   # Deploy to node (runs compile first)
   # First time: use deploy serial to flash via USB
   deploy serial                       # Via local USB (first time)
   
   # Subsequent deploys (OTA - over the air, finds IP automatically)
   deploy                              # All nodes in current folder
   deploy 192.168.1.50                 # Specific IP
   deploy rfc2217://IP-addr.local:port # Via remote serial over network

Monitoring
~~~~~~~~~~

.. code-block:: bash

   # Monitor MQTT messages (run in system folder or node folder)
   mqtt_listen
   
   # Serial console
   serial_console


Code Generation Flow
--------------------

.. code-block::

   setup.cpp
      ↓
   [Tokenize] command_checker.py
      ↓
   [Parse] devices.ini
      ↓
   [Generate] devices_generated.h
      ↓
   [Include] Only needed device drivers
      ↓
   [Compile] PlatformIO
      ↓
   firmware.bin


Device Macro System
-------------------

User Code
~~~~~~~~~

.. code-block:: cpp

   led(status, D1);  // Note: name without quotes!

Macro Expansion
~~~~~~~~~~~~~~~

.. code-block:: cpp

   // Step 1: Alias resolution (led → output)
   output(status, D1);
   
   // Step 2: Device macro (creates internal name)
   IOTEMPOWER_DEVICE(status, output_, D1);
   
   // Step 3: Expands to IOTEMPOWER_DEVICE_(Output, iotempower_dev_status, "status", D1)
   
   // Step 4: Final code
   Output iotempower_dev_status __attribute__((init_priority(65535))) 
       = Output("status", D1);
   Output& status = (Output&) iotempower_dev_status;

Result: 
- Global device instance named ``iotempower_dev_status``
- Reference ``status`` for code access
- Use ``IN(status)`` macro to get the internal name: ``iotempower_dev_status``

The IN() Macro
~~~~~~~~~~~~~~

The ``IN()`` macro (Internal Name) is used to reference devices in code.

Example from Sonoff Basic (see examples/sonoff/basic):

.. code-block:: cpp

   // Device definitions
   led(green, GREENLED).inverted().report_change(false);
   output(relais1, RELAIS1).off();
   input(button1, BUTTON1, "released", "pressed")
       .debounce(3)
       .on_change([] (Device& dev) {
            if(dev.is("pressed")) {
                IN(relais1).toggle();  // Use IN() to access relais1
                IN(green).toggle();    // Use IN() to access green LED
            }
            return true;
        });
   
   // IN(relais1) expands to: iotempower_dev_relais1
   // IN(green) expands to: iotempower_dev_green


Creating New Device Types
--------------------------

1. Choose Base Class
~~~~~~~~~~~~~~~~~~~~

- **Device**: Simple device
- **Input_Base**: Input sensor
- **I2C_Device**: I2C sensor
- **RGB_Base**: RGB lighting

2. Create Header
~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // lib/node_types/esp/src/dev_mydevice.h
   #ifndef _MYDEVICE_H_
   #define _MYDEVICE_H_
   
   #include <device.h>
   
   class MyDevice : public Device {
   private:
       int _pin;
   public:
       MyDevice(const char* name, int pin);
       void start() override;
       bool measure() override;
   };
   
   #endif

3. Create Implementation
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // lib/node_types/esp/src/dev_mydevice.cpp
   #include "dev_mydevice.h"
   
   MyDevice::MyDevice(const char* name, int pin) 
       : Device(name, 10000) {
       _pin = pin;
       add_subdevice(new Subdevice());
   }
   
   void MyDevice::start() {
       pinMode(_pin, INPUT);
       _started = true;
   }
   
   bool MyDevice::measure() {
       int val = digitalRead(_pin);
       value().from(val);
       return true;
   }

4. Register in devices.ini
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: ini

   [mydevice]
   aliases = mydev
   filename = mydevice
   lib = some/library@^1.0.0

5. Use in setup.cpp
~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   mydevice(sensor1, D1);  // Note: name without quotes!


Common Patterns
---------------

Callbacks
~~~~~~~~~

.. code-block:: cpp

   // On value change - use IN() to access device references
   IN(button).with_on_change_callback(*new Callback([](Device& dev) {
       if (dev.is("on")) {
           IN(led).on();
       }
       return true;
   }));

Filters
~~~~~~~

.. code-block:: cpp

   // Only publish if changed by threshold
   sensor.with_filter_callback(*new Callback([](Device& dev) {
       float current = dev.value().as_float();
       float last = dev.get_last_confirmed_value().as_float();
       return abs(current - last) >= 0.5;
   }));

Scheduled Actions
~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // Do something later - use IN() to access devices
   do_later(5000, []() {
       IN(led).off();
   });


Troubleshooting Quick Tips
---------------------------

Compilation Errors
~~~~~~~~~~~~~~~~~~

- **Unknown device**: Check spelling, verify in devices.ini
- **Missing include**: Device not detected in setup.cpp
- **Syntax error**: Check device parameters and syntax

Deployment Errors
~~~~~~~~~~~~~~~~~

- **Can't connect**: Check WiFi, verify IP address
- **Upload fails**: Try serial deployment
- **Device offline**: Check power, WiFi signal

Runtime Issues
~~~~~~~~~~~~~~

- **No MQTT messages**: Check broker connection, topic names
- **Sensor not working**: Verify pin numbers, check wiring
- **Crashes/resets**: Check memory usage, remove debug code


Performance Tips
----------------

- **Optimize pollrates**: Don't poll sensors faster than needed
- **Use filters**: Reduce MQTT traffic by filtering unchanged values
- **Buffer inputs**: Use precision buffers for noisy sensors
- **Batch updates**: Let system handle publishing, don't force publish


Further Documentation
---------------------

Detailed Guides
~~~~~~~~~~~~~~~

- `Architecture Overview <architecture.rst>`_ - System design
- `Device Architecture <device-architecture.rst>`_ - Device framework
- `Deployment Process <deployment-process.rst>`_ - Build workflow

Command References
~~~~~~~~~~~~~~~~~~

- `Command Reference <node_help/commands.rst>`_ - All available devices
- `Project Reference <projects_help/projects.rst>`_ - Example projects

Hardware
~~~~~~~~

- `Supported Hardware <hardware.rst>`_ - Compatible boards
- `Installation <installation.rst>`_ - Setup instructions


Summary
-------

IoTempower provides:

- **High-level Configuration**: Simple setup.cpp syntax
- **Automatic Code Generation**: Only includes needed drivers
- **OTA Updates**: Deploy wirelessly
- **MQTT Integration**: Automatic publishing and subscribing
- **Extensible**: Easy to add new devices
- **Production-ready**: Reliable and well-tested

Start with the `First Node <first-node.rst>`_ guide to build your first IoT device!
