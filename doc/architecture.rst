Architecture
============

System Overview
---------------

.. figure:: /doc/images/system-architecture.png
   :width: 50%
   :figwidth: 100%
   :align: center
   :alt: IoTempower System Architecture

   IoTempower System Architecture

In an IoTempower system you find one or several IoTempower gateways (the edge)
and wirelessly connected nodes - usually microcontrollers like the Wemos D1 mini
or Nodemcu. This microcontrollers have physically connected devices: sensors or actors
(also called things in an Internet of Things architecture).

An *IoTempower gateway* contains and runs all the software needed to configure an
IoT system. It also provides facilities to run and manage a WiFi router as well
as an MQTT-broker. It therefore provides configuration management software as
well as dataflow management services. However, if you can run IoTempower also
on your own dedicated Linux system (or even on WSL 1) and have other gateway
services separately. So you could have a wifi router providing your 
IoT edge network and an MQTT-broker (like mosquitto) running on this router,
another dedicated Linux installation, or inside your dedicated Linux system.conf
Depending on your choice, you therefore might separate WiFi router, broker,
and deployment. Your integration logic like 
`Node-RED <https://nodered.org/>`__ or 
`Home-Assistant <https://www.home-assistant.io/>`__
might run on another dedicated Linux installation or on any of the
existing.

An integrated gateway but also any of the single services can for example easily be installed on a Raspberry Pi 3 or 4.
*Nodes* are wireless components which interact with physical objects (things).
Usually a *node* has several *devices* (things) attached to it.
Do not confuse these hardware nodes with the nodes of a flow in Node-RED.
*devices* can be sensors (like a button or
temperature, light, movement, or humidity sensor)
or actors (like relais, solenoids,
motors, or LEDs).


IoTempower as a Framework
--------------------------

IoTempower operates on multiple levels:

1. **Device Management Framework**
   
   IoTempower provides a comprehensive C++ framework for managing IoT devices on microcontrollers:
   
   - Object-oriented device architecture with inheritance
   - Automatic device registration and lifecycle management
   - Standardized interfaces for sensors and actuators
   - Built-in MQTT publishing and command reception
   - Callback system for device interactions
   
   See: `Device Architecture <device-architecture.rst>`_ for detailed information.

2. **Network and Systems Integration**
   
   IoTempower integrates all components of an IoT system:
   
   - WiFi access point management
   - MQTT broker setup and configuration
   - Network topology and routing
   - Service discovery and registration
   - Integration with home automation platforms

3. **Configuration and Deployment Management**
   
   IoTempower automates the build and deployment process:
   
   - High-level device configuration language
   - Automatic code generation from device specifications
   - Dependency resolution and library management
   - Over-the-air (OTA) firmware updates
   - Multi-node system management
   
   See: `Deployment Process <deployment-process.rst>`_ for detailed information.


Architecture Layers
-------------------

IoTempower's architecture consists of several layers:

.. code-block::

   ┌─────────────────────────────────────────────┐
   │  Integration Layer                          │
   │  (Node-RED, Home Assistant, Custom Apps)    │
   └─────────────────────────────────────────────┘
                      ↕ MQTT
   ┌─────────────────────────────────────────────┐
   │  Gateway Layer                              │
   │  • WiFi Router                              │
   │  • MQTT Broker                              │
   │  • Configuration Management                 │
   └─────────────────────────────────────────────┘
                      ↕ WiFi + MQTT
   ┌─────────────────────────────────────────────┐
   │  Node Layer (ESP8266/ESP32)                 │
   │  • Network Connection                       │
   │  • MQTT Client                              │
   │  • Device Manager                           │
   │  • Main Loop                                │
   └─────────────────────────────────────────────┘
                      ↕ GPIO/I2C/SPI/etc.
   ┌─────────────────────────────────────────────┐
   │  Device Layer                               │
   │  • Sensors (temperature, motion, etc.)      │
   │  • Actuators (LEDs, relays, motors)         │
   │  • Displays, RFID readers, etc.             │
   └─────────────────────────────────────────────┘


Node Architecture
-----------------

Each IoTempower node runs firmware with this structure:

Main Loop
~~~~~~~~~

.. code-block:: cpp

   void loop() {
       // 1. Handle network connectivity
       maintain_wifi_connection();
       
       // 2. Handle MQTT connection
       maintain_mqtt_connection();
       
       // 3. Update all devices
       device_manager.update(in_precision_interval);
       
       // 4. Publish changed values
       device_manager.publish(mqtt_client, node_topic);
       
       // 5. Handle incoming MQTT messages
       mqtt_client.loop();
       
       // 6. Execute scheduled callbacks
       do_later_check();
   }

Device Manager
~~~~~~~~~~~~~~

The Device Manager (singleton) orchestrates all devices:

- **Registration**: Devices auto-register during global initialization
- **Lifecycle**: Calls ``start()`` on all devices during setup
- **Polling**: Calls ``measure()`` at device-specific intervals
- **Publishing**: Publishes changed values via MQTT
- **Commands**: Routes incoming MQTT messages to devices

Device Instances
~~~~~~~~~~~~~~~~

Each device in your ``setup.cpp`` becomes a global object:

.. code-block:: cpp

   // In setup.cpp
   led("status", D1);
   dht22("temp", D4);
   
   // Generates (simplified)
   Output status_device("status", D1);
   Dht22 temp_device("temp", D4);
   
   // Registered in DeviceManager automatically

MQTT Topics
~~~~~~~~~~~

Each device publishes and subscribes to specific topics:

.. code-block::

   <system>/<node>/<device>/[subdevice]/[command]
   
   Examples:
   my-home/living-room/temp/temperature        → 23.5 (published)
   my-home/living-room/temp/humidity           → 45 (published)
   my-home/living-room/status/set              ← on (subscribed)


Configuration Management
------------------------

System Hierarchy
~~~~~~~~~~~~~~~~

.. code-block::

   IoT System (e.g., "my-home")
   ├── system.conf                    # Network and MQTT config
   ├── Node 1 (e.g., "living-room")
   │   ├── node.conf                  # Board type
   │   ├── setup.cpp                  # Device configuration
   │   └── build/                     # Generated files
   │       ├── src/
   │       │   ├── main.cpp           # Main firmware
   │       │   ├── devices_generated.h # Device includes
   │       │   └── setup.cpp          # Your config
   │       └── platformio.ini         # Build config
   └── Node 2 (e.g., "bedroom")
       └── ...

Configuration Flow
~~~~~~~~~~~~~~~~~~

1. **System Level** (system.conf)
   
   - Network SSID and password
   - MQTT broker address
   - System-wide defaults

2. **Node Level** (node.conf)
   
   - Board type (esp8266, esp32, etc.)
   - Node-specific overrides

3. **Device Level** (setup.cpp)
   
   - Which devices are connected
   - Device parameters and behavior
   - Inter-device logic


Device Driver Architecture
--------------------------

Base Classes
~~~~~~~~~~~~

IoTempower provides several base classes for device types:

- **Device**: Base class for all devices
- **Input_Base**: Base for all input devices (buttons, sensors)
- **I2C_Device**: Base for I2C-connected devices
- **RGB_Base**: Base for RGB lighting devices

Inheritance Example
~~~~~~~~~~~~~~~~~~~

.. code-block::

   Device (abstract base)
   └── I2C_Device (I2C communication support)
       └── Barometer_BMP180 (specific sensor)
           • Temperature reading
           • Pressure reading
           • Altitude calculation

Each level adds functionality:

1. **Device**: Name, polling, MQTT, lifecycle
2. **I2C_Device**: I2C bus management, addressing, scanning
3. **Barometer_BMP180**: BMP180-specific sensor communication

See `Device Architecture <device-architecture.rst>`_ for complete details.


Code Generation System
----------------------

The code generation system makes IoTempower easy to use:

1. **Device Detection**
   
   Analyzes your ``setup.cpp`` to find which devices you're using

2. **Dependency Resolution**
   
   Determines which device drivers and libraries are needed

3. **Code Generation**
   
   Creates ``devices_generated.h`` with only needed code

4. **Library Management**
   
   Generates ``platformio-libs.ini`` with required libraries

5. **Compilation**
   
   PlatformIO compiles optimized firmware

Benefits:

- **Minimal firmware size**: Only includes used drivers
- **Fast compilation**: Only compiles needed code  
- **Easy extension**: Add new devices by editing devices.ini
- **Type safety**: C++ classes provide compile-time checking

See `Deployment Process <deployment-process.rst>`_ for complete details.


Integration Patterns
--------------------

Home Assistant
~~~~~~~~~~~~~~

IoTempower nodes automatically integrate with Home Assistant via:

- MQTT Discovery: Devices auto-register
- Entity types: Sensors, switches, lights, etc.
- State updates: Real-time via MQTT
- Commands: Control devices via MQTT

Node-RED
~~~~~~~~

IoTempower works seamlessly with Node-RED:

- MQTT nodes subscribe to device topics
- Function nodes process data
- Dashboard nodes for UI
- Rule engine for automation

Custom Applications
~~~~~~~~~~~~~~~~~~~

You can build custom applications that:

- Subscribe to device topics via MQTT
- Send commands to devices
- Process and store data
- Implement custom logic


Extensibility
-------------

Adding New Device Types
~~~~~~~~~~~~~~~~~~~~~~~

1. Create C++ class inheriting from appropriate base
2. Implement lifecycle methods (start, measure)
3. Register in devices.ini
4. Document in node_help/

Adding New Board Types
~~~~~~~~~~~~~~~~~~~~~~

1. Create directory in lib/node_types/
2. Provide platformio.ini configuration
3. Optionally override default pin mappings

Adding New Features
~~~~~~~~~~~~~~~~~~~

The modular architecture allows:

- Custom callbacks for device interactions
- Filters for value processing
- New communication protocols
- Additional services on nodes


Summary
-------

IoTempower provides a complete IoT framework combining:

- **Device Management**: Object-oriented C++ device architecture
- **Network Integration**: WiFi, MQTT, service discovery
- **Configuration Management**: High-level device specification
- **Deployment Automation**: Automated build and OTA updates
- **Extensibility**: Easy to add new devices and features

This multi-layered architecture enables rapid development of reliable IoT systems.


Further Reading
---------------

- `Device Architecture <device-architecture.rst>`_ - Detailed device framework documentation
- `Deployment Process <deployment-process.rst>`_ - Complete deployment workflow
- `Command Reference <node_help/commands.rst>`_ - Available device types
- `Supported Hardware <hardware.rst>`_ - Compatible boards and sensors


Top: `ToC <index-doc.rst>`_, Previous: `Introduction <introduction.rst>`_,
Next: `Supported Hardware <hardware.rst>`_