# GitHub Copilot Instructions for IoTempower

## Repository Overview

IoTempower is an IoT framework for rapid deployment and management of wireless sensor/actuator networks using ESP8266/ESP32 microcontrollers. It provides device drivers, MQTT integration, OTA updates, and a configuration-based deployment system.

## Key Architecture Concepts

### System Hierarchy
```
IoT System (e.g., ~/iot-systems/my-home/)
├── system.conf              # System-wide configuration (WiFi, MQTT broker)
└── node-name/               # Each microcontroller has its own folder
    ├── node.conf            # Board type and node-specific settings
    ├── setup.cpp            # Device configuration (sensors/actuators)
    ├── key.txt              # Security key for OTA updates
    └── build/               # Auto-generated PlatformIO project (transient)
```

**Important**: Nodes and devices are defined declaratively in `setup.cpp` using a high-level C++ DSL. The system automatically handles MQTT topics, networking, security, and OTA updates.

## Directory Structure

### Documentation (`/doc`)
- `quickstart-pi.rst` - Quick start guide for Raspberry Pi image
- `architecture.rst` - System architecture overview
- `architecture-quick-reference.rst` - Quick reference for key concepts
- `first-node.rst` / `second-node.rst` - Essential tutorials
- `device-architecture.rst` - Device framework details
- `deployment-process.rst` - Build and deployment workflow
- `node_help/` - Device command references
- `projects_help/` - Project configuration help

### System Management (`/bin`)
- `compile` - Compiles node firmware
- `deploy` - Deploys firmware (serial or OTA)
- `command_checker.py` - Detects which device drivers are needed
- `create_node_template` - Creates new node folders
- Other gateway management scripts (accesspoint, MQTT, etc.)

### MCU Device Drivers (`/lib/node_types/esp/src`)
**Critical**: This folder contains the base device drivers and uses an **implicit inheritance structure** through symbolic links:

- `device.h/.cpp` - Base Device class (all devices inherit from this)
- `dev_*.h/.cpp` - Individual device drivers (sensors, actuators, I2C devices, etc.)
- `devices.ini` - Device metadata and dependencies
- `main.cpp` - Main loop (handles polling, MQTT, OTA)
- `setup.cpp` - Template that includes user's `setup.cpp` as `setup.h` (via symbolic link in build directory)

**Implicit Inheritance via Symbolic Links**:
Board-specific folders inherit from parent folders through `base` symbolic links:
- `esp32/base` → `esp` (ESP32 boards inherit from esp folder, overriding files as needed)
- `m5stickc/base` → `esp32` (M5StickC inherits from esp32, which inherits from esp)
- `esp8266/base` → `esp` (ESP8266 boards inherit from esp folder)
- `wemos_d1_mini/base` → `nodemcu` → `esp8266` → `esp` (chain of inheritance)

During deployment, files are copied following this inheritance chain, with board-specific files overriding parent files.

When `deploy` is called, the system:
1. Analyzes the node's `setup.cpp` to detect which devices are used
2. Generates `devices_generated.h` with required includes
3. Assembles a complete PlatformIO project in `node-folder/build/`
4. Copies source files following the symbolic link inheritance structure (board-specific files override parent files)
5. Creates a symbolic link from user's `setup.cpp` to `setup.h` in the build directory
6. Compiles and uploads firmware

## Writing setup.cpp Files

### Device Naming Convention
**Do NOT quote device names** - they come from command auto-expansion:
```cpp
// Correct - device names are NOT strings
output(blue_led, ONBOARDLED).off();
input(button1, D3, "up", "down");
analog(temp_sensor).with_threshold(30, "hot", "cold");

// Incorrect - do not use quoted names
// output("blue_led", ONBOARDLED);  // WRONG
```

### MQTT is Automatic
**Do NOT manually configure MQTT** - the system handles it automatically:
- MQTT topics follow the folder hierarchy: `folder-path/device_name/[subtopic]`
- For a node folder at `a/b/c/node.conf`, devices publish to `a/b/c/device_name`
- The folder name containing `node.conf` becomes the node name in the MQTT topic path
- Example: A device configured as `blue_led` in folder `a/b/c` publishes its status to `a/b/c/blue_led`
- Commands are received at `a/b/c/blue_led/set`
- WiFi credentials and MQTT broker are configured in `system.conf`

### Device Declaration Patterns
Look at `/examples` to understand patterns:
```cpp
// Simple devices
output(relay, RELAIS1).off();
led(status, ONBOARDLED).invert().off();
input(button, D3).invert().debounce(10);

// Sensors with callbacks
analog(moisture)
    .with_threshold(100, "wet", "dry")
    .with_on_change_callback([] {
        if(IN(moisture).value().equals("wet")) {
            IN(relay).on();
        }
    });

// Complex devices with filters
input(touch1, BUTTON1)
    .filter_click_detector(20, 800, 1000, 2500)
    .on_change([] (Device& dev) {
        if(dev.is("click")) {
            IN(relay).toggle();
        }
        return true;
    });
```

### Device Access
- Use `IN(device_name)` macro to access device instances (expands to `iotempower_dev_device_name`)
- The `IN()` macro provides access to the actual Device object reference
- Common methods: `.set()`, `.on()`, `.off()`, `.toggle()`, `.value()`, `.is()`

## Common Device Types

### Input Devices
- `input(name, pin, off_msg, on_msg)` - Digital input/button
- `analog(name, pin)` - Analog input (ADC)

### Output Devices
- `output(name, pin)` - Digital output/relay
- `led(name, pin)` - LED with color effects
- `pwm(name, pin)` - PWM output
- `servo(name, pin)` - Servo motor

### I2C Sensors
- `dht(name, pin, type)` - DHT11/DHT22 temperature/humidity
- `i2c_bmp180(name)` - Barometer/temperature
- `i2c_bh1750(name)` - Light sensor
- `ds18b20(name, pin)` - OneWire temperature

### RGB/LED Strips
- `rgb_single(name, pin_r, pin_g, pin_b)` - Single RGB LED
- `rgb_strip(name, pin, led_count)` - WS2812/NeoPixel strips

See `/doc/node_help/` for complete device documentation.

## Build and Deploy Process

### Compile Only
```bash
cd ~/iot-systems/my-home/node1
compile  # or: deploy compile
```

### Deploy via Serial
```bash
deploy serial  # Auto-detects USB port
# or specify: deploy serial /dev/ttyUSB0
```

### Deploy Over-The-Air (OTA)
```bash
deploy  # Automatically uses OTA if node is online
```

### Adopting Existing Nodes
For nodes already flashed with IoTempower:
```bash
cd node-folder
adopt  # or: initialize
# Follow prompts to configure WiFi and complete setup
```

## Important Coding Guidelines

### When Modifying Device Drivers (`/lib/node_types/esp/src/`)
1. All devices inherit from `Device` base class
2. Override `start()` for initialization, `measure()` for polling
3. Return `true` from `measure()` to mark that value can be published
4. Add device metadata to `devices.ini` for dependency tracking
5. Test with multiple board types (esp8266, esp32)

### When Modifying Build Scripts (`/bin/`)
1. Maintain compatibility with both Raspberry Pi image and standalone Linux
2. Scripts must work from both node folders and system folders
3. Use `IOTEMPOWER_ROOT` and `IOTEMPOWER_ACTIVE` environment variables
4. Handle errors gracefully (users may not have all dependencies)

### When Adding Examples
1. Create folder with descriptive name in `/examples/`
2. Include `node.conf` specifying board type
3. Add well-commented `setup.cpp` showing device usage
4. Document pin connections in comments or README

## Testing Changes

### Before Committing
1. Use the testing infrastructure: `iot test compile` (or `pytest -s -v test_compilation.py`)
2. Test specific boards/devices: `iot test compile --boards=wemos_d1_mini,esp32 --devices=laser_distance`
3. If possible, test deployment on actual hardware
4. Verify documentation builds: `cd doc && make html`
5. See `/doc/testing.rst` and `/doc/github-actions.rst` for full testing documentation

## Reference Documentation

**Must read before significant changes:**
- `/doc/quickstart-pi.rst` - Quick start guide
- `/doc/architecture-quick-reference.rst` - Quick reference for architecture
- `/doc/architecture.rst` - Complete system architecture
- `/doc/first-node.rst` - Basic workflow and concepts
- `/doc/second-node.rst` - OTA deployment patterns
- `/doc/device-architecture.rst` - Device framework internals
- `/doc/deployment-process.rst` - Build system details

## Quick Tips

- Device driver code is in `/lib/node_types/esp/src/dev_*.cpp`
- User-facing documentation is in `/doc/*.rst` (ReStructuredText)
- System commands are in `/bin/` (mostly Bash scripts)
- Example configurations are in `/examples/`
- PlatformIO configuration is in `/lib/node_types/esp/platformio.ini`
- The `build/` folder in each node is transient - never edit it directly
- MQTT topics follow the folder hierarchy pattern: `folder/path/node/device`
- All networking, security, and MQTT is handled automatically by the framework
- User's `setup.cpp` is included in `main.cpp` via `setup.h` symbolic link created during build
