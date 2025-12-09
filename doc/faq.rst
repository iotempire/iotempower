==========================
Frequently Asked Questions
==========================

This document answers common questions about IoTempower. If you don't find your answer here, check our `support page <support.rst>`_ for other ways to get help.

.. contents:: Topics
   :local:
   :depth: 2

Getting Started
===============

What is IoTempower?
-------------------

IoTempower is a framework for easily creating, deploying, and managing Internet of Things (IoT) systems. It provides tools for configuring wireless sensor/actuator nodes, managing IoT gateways, and integrating with home automation platforms like Home Assistant and Node-RED.

See our `introduction <introduction.rst>`_ for more details.


What hardware do I need?
-------------------------

At minimum, you need:

- **A gateway device**: A Linux computer (like a Raspberry Pi) or a Linux system (native, WSL2, or container)
- **IoT nodes**: ESP8266 or ESP32 microcontroller boards (like Wemos D1 Mini, NodeMCU, ESP32 DevKit)
- **Sensors/actuators**: Depending on your project (buttons, LEDs, temperature sensors, relays, etc.)

Check our `hardware guide <hardware.rst>`_ for recommended devices and compatibility information.


Which operating systems are supported?
---------------------------------------

IoTempower works on:

- **Linux** (Ubuntu, Debian, Arch, Manjaro) - recommended, best experience
- **macOS** (with Homebrew)
- **Windows** via WSL2 (Windows Subsystem for Linux)
- **Android** via Termux
- **Containerized** via Podman (Docker is deprecated)

See our `installation guide <installation.rst>`_ for platform-specific instructions.


Installation & Setup
====================

How do I install IoTempower?
-----------------------------

The quickest way is using the installation script:

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -s -- --default

For detailed instructions and alternative methods, see our `installation guide <installation.rst>`_.


Where is IoTempower installed by default?
------------------------------------------

The default installation directory is ``/iot``. You can customize this during installation if needed.


How do I update IoTempower?
----------------------------

Run the following command from within your IoTempower installation:

.. code-block:: bash

   iot upgrade

This will pull the latest changes and update your installation.


Device Configuration
====================

How do I add a new device/sensor?
----------------------------------

To add a device to a node:

1. Navigate to your node directory (e.g., ``/iot/projects/myproject/mynode``)
2. Edit the ``setup.cpp`` file
3. Add device initialization code using IoTempower's device classes
4. Run ``iot compile`` to verify your configuration
5. Run ``iot deploy`` to flash the updated firmware

For device-specific examples, check our `device source code <../lib/node_types/esp/src>`_ and `architecture guide <device-architecture.rst>`_.


What devices/sensors are supported?
------------------------------------

IoTempower supports a wide range of devices including:

- **Digital I/O**: Buttons, switches, relays, LEDs
- **Analog sensors**: Light sensors, potentiometers
- **Temperature/Humidity**: DHT11, DHT22, DS18B20, BMP280, BME280
- **Distance**: HC-SR04 ultrasonic, VL53L0X laser
- **Light**: BH1750, TSL2561
- **Motion**: MPU6050, MPU9250 gyroscope/accelerometer
- **Display**: I2C OLED displays
- **RGB LEDs**: WS2812B, NeoPixels
- And many more...

Browse the `lib/node_types/esp/src/` directory or check our documentation for a complete list.


How do I create support for a new device type?
-----------------------------------------------

To add support for a new device:

1. Study the `device architecture <device-architecture.rst>`_ to understand the class hierarchy
2. Look at existing device implementations in `lib/node_types/esp/src/`
3. Create a new device class inheriting from appropriate base classes
4. Implement required methods: ``start()``, ``measure()``, ``publish()``
5. Test your device with a real hardware setup
6. Submit a pull request with your implementation

See our `contributing guide <../.github/CONTRIBUTING.md>`_ for more details.


Network & Connectivity
======================

How do I connect my nodes to WiFi?
-----------------------------------

IoTempower can set up its own WiFi access point, or your nodes can connect to an existing network. Configuration is done in the project's ``node.conf`` file.

See the `first node tutorial <first-node.rst>`_ for step-by-step instructions.


What is MQTT and why does IoTempower use it?
---------------------------------------------

MQTT is a lightweight messaging protocol perfect for IoT. IoTempower uses MQTT to:

- Send sensor data from nodes to your applications
- Send commands to actuators on your nodes
- Integrate with home automation platforms

IoTempower can run an MQTT broker on your gateway, or connect to an external broker.


Can I use IoTempower without an MQTT broker?
---------------------------------------------

No, MQTT is central to IoTempower's architecture. However, IoTempower can automatically set up and manage an MQTT broker for you, so you don't need to configure one separately.


Troubleshooting
===============

My node won't flash/connect - what should I check?
---------------------------------------------------

Common issues:

1. **USB connection**: Ensure the USB cable supports data (not just power)
2. **Drivers**: Install CH340 or CP2102 USB-to-serial drivers if needed
3. **Permissions**: Add your user to the ``dialout`` group on Linux: ``sudo usermod -a -G dialout $USER``
4. **Boot mode**: Some boards need a button pressed during flashing
5. **Port selection**: Use ``iot`` command to select the correct serial port

For network flashing issues, verify your node is connected to the correct WiFi network.


How do I see debug output from my node?
----------------------------------------

Use the serial monitor:

.. code-block:: bash

   iot console

This will show serial output from your node, including startup messages, connection status, and debug information.


My sensor readings seem wrong or unstable - why?
-------------------------------------------------

Common causes:

1. **Power issues**: Ensure adequate power supply (USB ports may not provide enough current)
2. **Wiring**: Check connections, especially ground and power
3. **Pull-up/pull-down resistors**: Some sensors require these
4. **Polling rate**: Adjust the poll rate if readings are too frequent
5. **Sensor calibration**: Some sensors need calibration for accurate readings

Check sensor datasheets and example code for proper usage.


Integration & Advanced Topics
==============================

How do I integrate with Home Assistant?
----------------------------------------

IoTempower supports MQTT discovery for Home Assistant. Enable it in your project configuration, and devices will automatically appear in Home Assistant.

See our documentation on integration platforms for details.


Can I use Node-RED with IoTempower?
------------------------------------

Yes! Node-RED works great with IoTempower. Use MQTT nodes in Node-RED to subscribe to sensor topics and publish commands to your actuators.


How do I run custom code on my nodes?
--------------------------------------

You can:

1. Edit the ``setup.cpp`` file in your node directory
2. Add custom initialization in the setup section
3. Use device callbacks for event-driven code
4. Modify the main loop if needed (advanced)

The `device architecture guide <device-architecture.rst>`_ explains the framework structure.


Security & Privacy
==================

Is the communication encrypted?
--------------------------------

MQTT communication can be encrypted using TLS. See our guide on `MQTT with TLS <mqtt-with-tls.rst>`_ for setup instructions.


How do I report security vulnerabilities?
------------------------------------------

**Do not** report security issues publicly. Contact the maintainer directly via the instructions in our `SECURITY.md <../.github/SECURITY.md>`_ file.


Contributing
============

How can I contribute to IoTempower?
------------------------------------

We welcome contributions! You can:

- Report bugs and request features via GitHub Issues
- Improve documentation
- Add support for new devices
- Fix bugs and implement features
- Share your projects and examples

Read our `contributing guide <../.github/CONTRIBUTING.md>`_ to get started. Look for issues labeled ``good-first-issue`` or ``help-wanted``.


I found a bug - what should I do?
----------------------------------

Open an issue on GitHub with:

- A clear description of the problem
- Steps to reproduce
- Expected vs. actual behavior  
- Your hardware and software versions
- Any error messages or logs

Use the bug report template for structured reporting.


Still Have Questions?
=====================

If your question isn't answered here:

- Check our `documentation index <index-doc.rst>`_
- Join our `Discord community <https://discord.gg/9gq8Q9p6r3>`_
- Ask on `GitHub Discussions <https://github.com/iotempire/iotempower/discussions>`_
- Open an `issue <https://github.com/iotempire/iotempower/issues>`_ if you found a bug or have a feature request

See our `support page <support.rst>`_ for all contact options.


Top: `ToC <index-doc.rst>`_, Previous: `Support <support.rst>`_
