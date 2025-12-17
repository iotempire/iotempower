==========
Quickstart
==========

This quickstart guide will help you set up IoTempower on your own computer
(Linux, Windows with WSL, or Mac) and configure your first IoT nodes. Unlike the 
older Raspberry Pi-based setup, this guide assumes you have your own home router
or a dedicated router for your IoT projects (such as students using a GL.iNet Mango
or similar OpenWRT router).

Prerequisites
=============

Before starting, you need:

- A computer running Linux, Windows 10/11 with WSL2, or macOS
- A WiFi router (your home router or a dedicated IoT router)
- An ESP8266 or ESP32 microcontroller board (e.g., Wemos D1 Mini, NodeMCU, ESP32 DevKit)
- A USB cable to connect your microcontroller
- Basic familiarity with the command line

If you're using a GL.iNet Mango or another OpenWRT router, that's great! We'll show
you how to run the MQTT broker directly on it for better performance.

Installation
============

First, install IoTempower on your computer following the `Installation Guide <installation.rst>`_.

Quick summary:

**On Linux, macOS, or WSL2 (Windows):**

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -

Hit enter on all questions to use default settings (recommended).

After installation, open a new terminal and run:

.. code-block:: bash

   iot

Your prompt should change to start with ``IoT`` - this means you're in the IoTempower environment.

Network Setup
=============

You have two main options for network setup:

Option 1: Home Router or Generic WiFi Router
--------------------------------------------

If you're using your home router or any standard WiFi router:

1. Make sure you know your WiFi network name (SSID) and password
2. Your router should provide DHCP (this is default for most routers)
3. You'll need to run an MQTT broker on your computer (we'll show you how below)

Option 2: OpenWRT Router (e.g., GL.iNet Mango)
----------------------------------------------

If you're using an OpenWRT router like the GL.iNet Mango MT300N v2:

1. Install mosquitto MQTT broker and its configuration interface on the router:

   - Log into your router's web interface
   - Go to System → Software
   - Update package lists
   - Install ``mosquitto-nossl`` and ``luci-app-mosquitto`` packages

2. Configure mosquitto permissions:

   - In the router's web interface, go to Services → mosquitto
   - Enable the service and configure it to allow anonymous connections
   - For more detailed OpenWRT mosquitto setup, see the `FAQ <faq.rst>`_

3. The router will typically have a fixed IP like ``192.168.8.1`` or ``192.168.1.1``
   (check your router's documentation)

4. In your IoTempower system configuration, you'll use ``_gateway`` as the MQTT host,
   which will automatically resolve to your router's IP

Starting Services
=================

Web Services (Caddy and Node-RED)
---------------------------------

IoTempower includes a convenient command to start Caddy (web server) and 
Node-RED (for visual IoT programming):

.. code-block:: bash

   iot x web_starter

This will start:

- **Caddy** web server on http://localhost:40080
- **Node-RED** on http://localhost:40080/nodered

Keep this terminal running. You can stop the services with Ctrl+C.

.. note::
   CloudCmd is not needed for this quickstart and is only included in the 
   Raspberry Pi image for convenience. You'll use local editors and the 
   ``iot menu`` command instead.

MQTT Broker (if not on router)
------------------------------

If you're **NOT** running mosquitto on your router, you need to start the MQTT broker
on your computer:

Open a **new terminal**, activate the IoTempower environment, and run:

.. code-block:: bash

   iot
   mqtt_starter scanif

The ``scanif`` option automatically detects your network interface and IP address.
Keep this terminal running.

If you **ARE** running mosquitto on your OpenWRT router, you can skip this step.

Creating Your First IoT System
==============================

System Folder Structure
-----------------------

IoTempower organizes IoT projects in **system folders**. Each system has:

- A ``system.conf`` file with network and MQTT settings
- Subfolders for each node (microcontroller)
- Each node has a ``setup.cpp`` file defining its devices

Let's create a new system called "my-home":

.. code-block:: bash

   cd ~/iot-systems
   create_system_template my-home
   cd my-home

This creates the system folder with a template configuration.

.. note::
   The ``~/iot-systems`` folder is created automatically during IoTempower 
   installation with default settings.

Configuring System Settings
---------------------------

Edit the ``system.conf`` file using your favorite editor:

**On Linux:**

.. code-block:: bash

   micro system.conf
   # or: nano system.conf
   # or: kate system.conf

**On Windows (WSL):**

.. code-block:: bash

   micro system.conf
   # or: nano system.conf
   # or use notepad++: notepad++.exe system.conf

Update the following settings:

.. code-block:: bash

   # Your WiFi network name
   IOTEMPOWER_AP_NAME="your-wifi-name"
   
   # Your WiFi password
   IOTEMPOWER_AP_PASSWORD="your-wifi-password"
   
   # MQTT broker location
   # If using OpenWRT router with mosquitto:
   IOTEMPOWER_MQTT_HOST="_gateway"
   
   # If running mqtt_starter on your computer, use your computer's IP:
   # Find it with: ipconfig (Windows) or ip a (Linux)
   # IOTEMPOWER_MQTT_HOST="192.168.1.100"
   # (replace with your actual IP - shown when mqtt_starter starts)
   # Note: You may need to allow port 1883 in your firewall

Save and close the file.

Creating Your First Node
========================

Create a new node in your system:

.. code-block:: bash

   cd ~/iot-systems/my-home
   create_node_template living-room-lamp
   cd living-room-lamp

This creates a new node folder with template files.

.. note::
   You can also use ``iot menu`` to create nodes interactively. Just run it
   in your system folder and select **"Create New Node Folder"** from the menu.

Configuring the Node
--------------------

Check the board type in ``node.conf``:

.. code-block:: bash

   micro node.conf

The default is usually ``wemos_d1_mini``. Change it if you're using a different board:

.. code-block:: bash

   board="wemos_d1_mini"
   # or: board="nodemcu"
   # or: board="esp32dev"
   # or: board="esp32minikit"

Defining Devices in setup.cpp
-----------------------------

Edit the ``setup.cpp`` file to define your devices:

.. code-block:: bash

   micro setup.cpp

For a simple example with a button and LED, add:

.. code-block:: cpp

   // Button on pin D3
   input(button1, D3, "released", "pressed").with_debounce(5);
   
   // Onboard LED (note: inverted for most ESP boards)
   output(blue_led, ONBOARDLED).inverted();

Save and close the file.

Flashing Your First Node
========================

Connect your microcontroller to your computer via USB.

Platform-Specific Serial Flashing
---------------------------------

**On Linux:**

1. Find your USB device:

   .. code-block:: bash

      ls /dev/ttyUSB*
      # or: ls /dev/ttyACM*

2. You may need to add yourself to the ``dialout`` group (one-time setup):

   .. code-block:: bash

      iot install --fix-serial
      # Then log out and log back in

   This should have been done during installation, but if you get permission 
   errors, run the command above.
   
   .. note::
      On Windows with WSL2, you need to use usbipd (see below).

3. Flash the node:

   .. code-block:: bash

      deploy serial

**On Windows (WSL2):**

1. Install usbipd-win (see `Installation Guide <installation.rst#usb-flashing-in-wsl2-with-usbipd-win>`_)

2. Use the WSL USB GUI or usbipd command line:
   
   - **Recommended**: Use the WSL USB GUI application to attach your device
   - The GUI makes it easy to manage USB devices and set permissions correctly
   - See the Installation Guide above for setup instructions

3. In WSL, the device should appear as ``/dev/ttyUSB0``

4. Flash the node:

   .. code-block:: bash

      deploy serial

**On macOS:**

1. Find your USB device:

   .. code-block:: bash

      ls /dev/cu.*
      # Usually /dev/cu.usbserial-* or /dev/cu.SLAB_USBtoUART

2. Flash the node:

   .. code-block:: bash

      deploy serial

**Alternative: Network Flashing via Router**

If you have an OpenWRT router with USB ports and ser2net configured:

.. code-block:: bash

   deploy serial rfc2217://192.168.8.1:5000

(Replace with your router's IP and configured port)

Monitoring Your Node
====================

After flashing, you can monitor your node in two ways:

Serial Console
--------------

In a new terminal:

.. code-block:: bash

   iot
   cd ~/iot-systems/my-home/living-room-lamp
   console_serial

You'll see debug output and device state changes.

MQTT Messages
-------------

In another terminal, navigate to your node folder and listen for MQTT messages:

.. code-block:: bash

   iot
   cd ~/iot-systems/my-home/living-room-lamp
   mqtt_listen

Now press the button on your microcontroller. You should see messages like:

.. code-block::

   living-room-lamp/button1 released
   living-room-lamp/button1 pressed

Congratulations! Your first node is working!

Over-the-Air Updates
====================

After the initial serial flash, you can update your node over WiFi.

1. Make changes to ``setup.cpp``
   
   For example, if you notice the button sometimes triggers multiple times 
   (button bouncing), increase the debounce time:

   .. code-block:: cpp

      // Change debounce from 5 to 10 milliseconds
      input(button1, D3, "released", "pressed").with_debounce(10);

2. Simply run:

   .. code-block:: bash

      deploy

No USB cable needed! The node will update itself over WiFi.

Controlling Devices via Node-RED
================================

Open Node-RED in your browser:

.. code-block::

   http://localhost:40080/nodered

Creating a Simple Flow
----------------------

1. Drag an **mqtt in** node to the canvas
2. Double-click it and set the topic to: ``living-room-lamp/button1``
3. Add a **debug** node and connect the mqtt node to it
4. Click **Deploy** (top right)
5. Open the debug panel (bug icon on the right)
6. Press your physical button - you'll see messages appear!

Controlling the LED
-------------------

1. Add another **mqtt in** node for ``living-room-lamp/button1``
2. Add a **change** node:
   - Set rule 1: Change "pressed" to "on"
   - Set rule 2: Change "released" to "off"
3. Add an **mqtt out** node with topic: ``living-room-lamp/blue_led/set``
4. Connect: mqtt in → change → mqtt out
5. Click **Deploy**

Now when you press the button, the LED should turn on and off!

.. note::
   This example connects button and LED on the same node, which might seem 
   confusing since the board is right in front of you. The real power comes 
   when you connect multiple nodes together! See the `Second Node Tutorial 
   <second-node.rst>`_ to experience the "magic" of controlling one device 
   from another across your network.

Your Second Node
================

Adding another node is easy - just create another node folder:

.. code-block:: bash

   cd ~/iot-systems/my-home
   create_node_template bedroom-sensor
   cd bedroom-sensor

Configure it, flash it, and then you can create flows in Node-RED to make 
your nodes interact with each other.

For example, a button on one node could control an LED on another node!

For more details, see the `Second Node Tutorial <second-node.rst>`_.

Tips and Best Practices
=======================

Using iot menu
--------------

The ``iot menu`` command is your friend for:

- Creating new nodes
- Deploying nodes
- Adopting pre-flashed nodes
- Managing your IoT system

Just run ``iot menu`` in any system or node folder.

Local Editors
-------------

Use these editors for a better experience:

- **Linux**: micro, nano, kate, gedit
- **Windows**: micro (in WSL), notepad++, VS Code
- **macOS**: micro, nano, TextEdit, VS Code

The micro editor is recommended as it's installed by IoTempower and
uses familiar keyboard shortcuts (Ctrl+S to save, Ctrl+Q to quit).

For more advanced development, consider using a full-fledged IDE like 
**VS Code**, **VSCodium**, or **Zed**. These provide powerful debugging 
capabilities. In particular, VS Code/VSCodium can work with the PlatformIO 
project found in the ``build`` folder of each node, giving you advanced 
features for troubleshooting complex issues.

Serial Port Access
------------------

If you get "Permission denied" errors when flashing:

- **Linux**: Run ``iot install --fix-serial``
- **Windows/WSL2**: Use the WSL USB GUI and enable the permission rule for WSL
- **macOS**: Usually works out of the box

Router Configuration
--------------------

Using ``_gateway`` in ``system.conf`` is useful when:

- Your router IP might change (e.g., Android hotspot)
- Multiple students share the same system configuration
- You want easier portability between networks

For home use with a static IP, you can use the actual IP address instead.

Next Steps
==========

Now that you have the basics working, explore more:

- Check the `Command Reference <node_help/commands.rst>`_ for more device types
  (temperature sensors, servos, displays, etc.)
- Learn about `Architecture <architecture.rst>`_ to understand how everything works
- Read about `Advanced Deployment <deployment-process.rst>`_ for more options
- Browse the `Examples <https://github.com/iotempire/iotempower/tree/master/examples>`_
  in the repository

For detailed information about specific topics:

- `Second Node Tutorial <second-node.rst>`_ - Connect multiple nodes
- `Hardware Support <hardware.rst>`_ - Supported boards and shields  
- `MQTT with TLS <mqtt-with-tls.rst>`_ - Secure MQTT communication
- `FAQ <faq.rst>`_ - Common questions and issues

Happy IoT building!

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
