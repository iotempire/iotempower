Deployment Process
==================

Overview
--------

IoTempower uses an automated build and deployment system that transforms high-level device configurations into optimized firmware for microcontrollers. The system analyzes your ``setup.cpp`` file, determines which device drivers are needed, generates appropriate code, and compiles a PlatformIO project.

This document explains the complete deployment workflow from configuration to running firmware.

Deployment Workflow
-------------------

The deployment process consists of several stages:

.. code-block::

   setup.cpp (user configuration)
        ↓
   [1] Command Detection (command_checker.py)
        ↓
   [2] Code Generation (devices_generated.h)
        ↓
   [3] Build Directory Preparation
        ↓
   [4] PlatformIO Compilation
        ↓
   [5] Firmware Upload (deploy/OTA)
        ↓
   Running Node


Directory Structure
-------------------

IoTempower organizes IoT systems in a hierarchical structure:

.. code-block::

   /iotempower/
   ├── lib/
   │   ├── node_types/          # Device driver library
   │   │   ├── esp/             # ESP8266/ESP32 support
   │   │   │   ├── src/         # Device source code
   │   │   │   │   ├── device.h/cpp
   │   │   │   │   ├── dev_*.h/cpp  # Device drivers
   │   │   │   │   └── devices.ini  # Device configuration
   │   │   │   └── platformio.ini
   │   │   ├── wemos_d1_mini/   # Board-specific configs
   │   │   └── ...
   │   └── system_template/     # Templates for new systems
   │       └── node_template/
   ├── bin/
   │   ├── compile              # Compile script
   │   ├── deploy               # Deploy script
   │   ├── prepare_build_dir    # Build preparation
   │   └── command_checker.py   # Device detection
   └── doc/
   
   /home/user/iot-systems/
   └── my-home/                 # System directory
       ├── system.conf          # System configuration
       ├── node1/               # Node directory
       │   ├── node.conf        # Node configuration
       │   ├── setup.cpp        # Device setup
       │   └── build/           # Generated build files
       └── node2/
           └── ...

System Configuration
--------------------

system.conf
~~~~~~~~~~~

Located in each system directory, defines system-wide settings:

.. code-block:: bash

   # System Configuration
   IOTEMPOWER_AP_NAME="my-iot-network"
   IOTEMPOWER_AP_PASSWORD="secret123"
   MQTT_HOST="192.168.1.100"
   MQTT_USER="mqtt"
   MQTT_PASSWORD="mqttpass"

Node Configuration
~~~~~~~~~~~~~~~~~~

node.conf
^^^^^^^^^

Located in each node directory, defines node-specific settings:

.. code-block:: bash

   # Node Configuration
   board="wemos_d1_mini"
   # or: board="esp32dev"

setup.cpp
^^^^^^^^^

The user-written file that defines which devices are connected:

.. code-block:: cpp

   // Example setup.cpp
   
   // LED on pin D1
   led("onboard_led", D1);
   
   // Button on pin D2 with pull-up
   button("door_button", D2).with_pull_up();
   
   // Temperature sensor on pin D4
   dht22("room_temp", D4);
   
   // I2C display on default pins
   display("status_display", 0x3c);


Stage 1: Command Detection
---------------------------

When you run ``compile`` or ``deploy``, the system first analyzes your ``setup.cpp`` to determine which device drivers are needed.

The command_checker.py Script
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Location: ``bin/command_checker.py``

This Python script:

1. **Parses devices.ini**
   
   Reads the device configuration file:
   
   .. code-block:: ini
   
      [dht22]
      filename = th
      lib = https://github.com/beegee-tokyo/DHTesp
      depends = ds18b20
      
      [display]
      filename = display_i2c
      class = Display44780_I2C

2. **Tokenizes setup.cpp**
   
   Extracts all identifiers (function/macro names) from your code

3. **Matches Commands**
   
   Finds which device commands (like ``dht22``, ``display``) are used

4. **Resolves Dependencies**
   
   Follows dependency chains (e.g., ``dht22`` depends on ``ds18b20``)

5. **Determines Required Files**
   
   Creates a list of ``dev_*.h`` files needed

Example Flow
^^^^^^^^^^^^

Given this setup.cpp:

.. code-block:: cpp

   led("status", D1);
   dht22("temp", D4);

The script determines:

- Commands found: ``led``, ``dht22``
- Aliases: ``led`` → ``output``
- Dependencies: ``dht22`` → ``ds18b20``
- Required files: ``output``, ``th`` (contains dht22 and ds18b20)
- Libraries needed: ``https://github.com/beegee-tokyo/DHTesp``, ``milesburton/DallasTemperature``, etc.


Stage 2: Code Generation
-------------------------

The command_checker.py script generates several files:

devices_generated.h
~~~~~~~~~~~~~~~~~~~

Location: ``<node>/build/src/devices_generated.h``

This header file includes only the needed device drivers and creates device macros:

.. code-block:: cpp

   // Generated automatically by command_checker.py
   
   // Begin: output
   #define IOTEMPOWER_COMMAND_OUTPUT
   #include <dev_output.h>
   #define output_(internal_name, ...) \
       IOTEMPOWER_DEVICE_(Output, internal_name, ##__VA_ARGS__)
   #define output(name, ...) IOTEMPOWER_DEVICE(name, output_, ##__VA_ARGS__)
   #define led_(gcc_va_args...) output_(gcc_va_args)
   #define led(gcc_va_args...) output(gcc_va_args)
   // End: output
   
   // Begin: dht22
   #define IOTEMPOWER_COMMAND_DHT22
   #include <dev_th.h>
   #define dht22_(internal_name, ...) \
       IOTEMPOWER_DEVICE_(Dht22, internal_name, ##__VA_ARGS__)
   #define dht22(name, ...) IOTEMPOWER_DEVICE(name, dht22_, ##__VA_ARGS__)
   // End: dht22

Device Macro System
~~~~~~~~~~~~~~~~~~~

The macro system provides clean syntax while handling device registration:

1. **User writes**: ``led("status", D1);``

2. **Expands to**: ``output("status", D1);``

3. **Expands to**: ``IOTEMPOWER_DEVICE(status, output_, D1);``

4. **Expands to**: 
   
   .. code-block:: cpp
   
      Output iotempower_dev_status __attribute__((init_priority(65535))) 
          = Output("status", D1);
      Output& status = (Output&) iotempower_dev_status;

This creates:
- A global device instance (``iotempower_dev_status``)
- With low initialization priority (runs last)
- And a reference (``status``) for easy access in code

platformio-libs.ini
~~~~~~~~~~~~~~~~~~~

Location: ``<node>/build/platformio-libs.ini``

Lists required PlatformIO libraries:

.. code-block:: ini

   [common]
     extra_lib_deps = 
       https://github.com/beegee-tokyo/DHTesp
       milesburton/DallasTemperature@^3.11.0
       paulstoffregen/OneWire@^2.3.7


Stage 3: Build Directory Preparation
-------------------------------------

The prepare_build_dir Script
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Location: ``bin/prepare_build_dir``

This bash script:

1. **Creates Build Directory**
   
   If it doesn't exist: ``<node>/build/``

2. **Links Node Type Files**
   
   Creates symlinks to the board-specific files:
   
   .. code-block::
   
      build/
      ├── src/              # Symlinked from lib/node_types/esp/src/
      ├── lib/              # Symlinked from lib/node_types/esp/lib/
      └── platformio.ini    # Generated

3. **Handles Base Inheritance**
   
   Node types can inherit from a base type:
   
   .. code-block::
   
      lib/node_types/
      ├── esp/              # Base type
      └── wemos_d1_mini/    # Specific board
          └── base -> ../esp  # Inherits from esp

   The script recursively follows base links to gather all necessary files.

4. **Copies User Configuration**
   
   Copies your ``setup.cpp`` to ``build/src/``

5. **Generates platformio.ini**
   
   Merges board-specific settings with generated library dependencies:
   
   .. code-block:: ini
   
      [platformio]
      ; Generated build configuration
      
      [env:esp8266]
      platform = espressif8266
      board = d1_mini
      framework = arduino
      
      ; Include generated libraries
      [include:platformio-libs.ini]
      
      ; System configuration
      build_flags = 
          -DIOTEMPOWER_AP_NAME="my-network"
          -DMQTT_HOST="192.168.1.100"
          ; ... (other config values)


Stage 4: PlatformIO Compilation
--------------------------------

Once the build directory is prepared, PlatformIO compiles the firmware:

.. code-block:: bash

   $ cd <node>/build
   $ platformio run

Compilation Process
~~~~~~~~~~~~~~~~~~~

1. **Install Dependencies**
   
   PlatformIO downloads required libraries

2. **Preprocess**
   
   C preprocessor expands all macros:
   
   - Device creation macros
   - Configuration defines
   - Conditional compilation directives

3. **Compile**
   
   Compiles all source files:
   
   - Main loop (``main.cpp``)
   - Device drivers (``dev_*.cpp``)
   - User setup (``setup.cpp``)
   - Library code

4. **Link**
   
   Links everything into a single firmware binary

5. **Generate Output**
   
   Creates:
   
   - ``firmware.bin`` - The main firmware
   - ``firmware.elf`` - Debug symbols
   - Memory usage report

Optimization
~~~~~~~~~~~~

The code generation system ensures:

- **Minimal Binary Size**: Only includes needed device drivers
- **Fast Compilation**: Uses PlatformIO's incremental builds and caching
- **Efficient Runtime**: Devices initialized at compile-time where possible


Stage 5: Firmware Upload
-------------------------

The deploy Script
~~~~~~~~~~~~~~~~~

Location: ``bin/deploy``

Deployment Methods:

1. **Over-the-Air (OTA)**
   
   For nodes already running IoTempower:
   
   .. code-block:: bash
   
      $ deploy                    # Deploy all nodes in system
      $ deploy 192.168.1.50       # Deploy to specific IP
      $ deploy node1              # Deploy specific node by name

2. **Serial Upload**
   
   For initial flashing or recovery:
   
   .. code-block:: bash
   
      $ deploy serial             # Auto-detect USB port
      $ deploy serial usb0        # Specific port
      $ deploy serial usb0 force  # Erase all first

3. **Adoption Mode**
   
   For adopting pre-configured nodes:
   
   .. code-block:: bash
   
      $ deploy adopt uiot-node-id-n1-m2

Deployment Workflow
~~~~~~~~~~~~~~~~~~~

1. **Compile** (if needed)

2. **Connect to Node**
   
   - Via network (OTA)
   - Via USB (serial)

3. **Upload Firmware**
   
   - Sends firmware.bin
   - Monitors upload progress

4. **Verify**
   
   - Waits for node to restart
   - Checks MQTT connectivity
   - Verifies device functionality

OTA Security
~~~~~~~~~~~~

OTA updates use:

- MD5 checksums for integrity
- Password authentication
- Encrypted communication (if configured)


Complete Example
----------------

Let's walk through a complete deployment:

1. Create System
~~~~~~~~~~~~~~~~

.. code-block:: bash

   $ cd ~/iot-systems
   $ system_create my-home
   $ cd my-home
   # Edit system.conf with network settings

2. Create Node
~~~~~~~~~~~~~~

.. code-block:: bash

   $ node_create living-room
   $ cd living-room

3. Configure Devices
~~~~~~~~~~~~~~~~~~~~

Edit ``setup.cpp``:

.. code-block:: cpp

   // Living room devices
   
   // Temperature and humidity sensor
   dht22("climate", D4);
   
   // Motion sensor
   input("motion", D5).with_pull_up();
   
   // LED strip
   rgb_strip("ambiance", D6, 30);
   
   // Setup reactions
   motion.with_on_change_callback(*new Callback([](Device& dev) {
       if (dev.is("on")) {
           ambiance.color(255, 255, 255);  // White on motion
       } else {
           ambiance.color(0, 0, 255);      // Blue when idle
       }
       return true;
   }));

4. Compile
~~~~~~~~~~

.. code-block:: bash

   $ compile

This:

- Analyzes setup.cpp
- Finds: dht22, input, rgb_strip
- Determines dependencies
- Generates devices_generated.h
- Creates build directory
- Compiles with PlatformIO

5. Deploy
~~~~~~~~~

First-time (serial):

.. code-block:: bash

   $ deploy serial
   
   # Device flashed, connects to WiFi
   # Shows: living-room is online at 192.168.1.123

Subsequent updates (OTA):

.. code-block:: bash

   $ deploy
   
   # Updates firmware over WiFi
   # Node reboots and reconnects

6. Monitor
~~~~~~~~~~

.. code-block:: bash

   $ console
   
   # Shows MQTT messages:
   # living-room/climate/temperature 23.5
   # living-room/climate/humidity 45
   # living-room/motion on
   # living-room/ambiance/color 255,255,255


Advanced Topics
---------------

Compile Cache
~~~~~~~~~~~~~

IoTempower uses a compile cache to speed up builds:

.. code-block:: bash

   # Cache location
   $IOTEMPOWER_COMPILE_CACHE
   
   # Multiple nodes can share cached objects
   # Only changed code is recompiled

Conditional Compilation
~~~~~~~~~~~~~~~~~~~~~~~~

Use preprocessor directives in setup.cpp:

.. code-block:: cpp

   #ifdef BOARD_ESP32
       // ESP32-specific code
   #endif
   
   #ifdef DEBUG_MODE
       // Debug-only devices
   #endif

Custom Build Flags
~~~~~~~~~~~~~~~~~~

Add to node.conf:

.. code-block:: bash

   board="wemos_d1_mini"
   build_flags="-DDEBUG_MODE -DCUSTOM_SETTING=123"

Troubleshooting
---------------

Common Issues
~~~~~~~~~~~~~

**Device not found in setup.cpp**

.. code-block::

   Error: Unknown device type 'dht11'

Solution: Check spelling, check if device exists in devices.ini

**Missing library**

.. code-block::

   Error: Library not found

Solution: Check internet connection, verify library name in devices.ini

**Compilation errors**

.. code-block::

   Error in setup.cpp line 10

Solution: Check device parameters, refer to command reference

**Upload fails**

.. code-block::

   Error: Failed to connect to device

Solution: Check WiFi, verify IP address, try serial upload


Summary
-------

The IoTempower deployment process:

1. **Analyzes** your device configuration
2. **Generates** optimized code including only needed drivers  
3. **Compiles** efficient firmware
4. **Deploys** via OTA or serial
5. **Runs** with automatic MQTT integration

This automation allows you to focus on defining what devices you want, not how to program them.


Next: See `Device Architecture <device-architecture.rst>`_ for details on device implementation.

See: `Command Reference <node_help/commands.rst>`_ for available device types.
