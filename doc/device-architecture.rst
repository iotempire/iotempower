Device Architecture
===================

Overview
--------

IoTempower uses an object-oriented C++ architecture for managing IoT devices. All devices inherit from a common base class hierarchy that provides standardized functionality for device initialization, measurement, publishing values via MQTT, and receiving commands.

This architecture enables rapid development of new device types while maintaining consistent behavior across the system.

Device Class Hierarchy
-----------------------

The device system is built on several key base classes that form an inheritance hierarchy:

.. code-block::

   Device (base class)
   ├── Input_Base
   │   ├── Input (digital input, button, contact)
   │   └── Analog (analog input)
   ├── Output (digital output, LED, relay)
   ├── PWM (pulse width modulation)
   ├── I2C_Device
   │   ├── Barometer_BMP180
   │   ├── Barometer_BMP280
   │   ├── Lux_BH1750
   │   ├── Lux_TSL2561
   │   ├── Distance_Vl53l0x
   │   └── ... (other I2C sensors)
   ├── Dht11 (temperature/humidity)
   ├── Dht22 (temperature/humidity)
   ├── Ds18b20 (temperature)
   ├── Hcsr04 (acoustic distance sensor)
   ├── RGB_Base
   │   ├── RGB_Single
   │   └── RGB_Strip_Bus
   └── ... (other device types)


The Device Base Class
----------------------

Location: ``lib/node_types/esp/src/device.h`` and ``device.cpp``

The ``Device`` class is the foundation of all IoTempower devices. It provides:

Core Functionality
~~~~~~~~~~~~~~~~~~

1. **Name and Identification**
   
   - Each device has a unique name used for MQTT topic creation
   - Names are stored as ``Ustring`` objects for memory efficiency

2. **Subdevices**
   
   - Devices can have multiple subdevices (e.g., a temperature sensor with both temperature and humidity readings)
   - Each subdevice can publish independent values
   - Managed through the ``Subdevice`` class

3. **Polling and Measurement**
   
   - Devices have configurable poll rates (``pollrate_us``)
   - The ``measure()`` method is called periodically to read sensor values
   - Virtual method allows each device type to implement custom measurement logic

4. **Publishing**
   
   - Values are published via MQTT when they change
   - The ``publish()`` method handles MQTT communication
   - Supports MQTT discovery for Home Assistant integration

5. **Command Reception**
   
   - Devices can subscribe to MQTT topics to receive commands
   - Subdevices can have individual receive callbacks
   - Commands are processed through ``receive_cb`` functions

6. **Lifecycle Methods**
   
   - ``start()``: Called during initialization to set up hardware
   - ``measure()``: Called periodically to read sensor values
   - ``buffer_reset()``: Resets any internal buffers for precise measurements

Key Methods
~~~~~~~~~~~

.. code-block:: cpp

   class Device {
   public:
       // Constructor with name and poll rate
       Device(const char* _name, unsigned long pollrate_us);
       
       // Lifecycle methods (virtual, can be overridden)
       virtual void start() { _started = true; }
       virtual bool measure() { return true; }
       virtual void buffer_reset() {}
       
       // Subdevice management
       Subdevice* add_subdevice(Subdevice* sd);
       Subdevice* subdevice(unsigned long index);
       
       // Value access
       Ustring& value(unsigned long index = 0);
       Ustring& get_last_confirmed_value(unsigned long index = 0);
       
       // Publishing
       bool needs_publishing();
       bool publish(PubSubClient& mqtt_client, Ustring& node_topic);
       
       // Configuration
       Device& set_pollrate_us(unsigned long rate_us);
       Device& with_pollrate_us(unsigned long rate_us);
   };

Device Registration
~~~~~~~~~~~~~~~~~~~

Devices are automatically registered with the ``DeviceManager`` singleton when constructed. The device manager handles:

- Starting all devices during system initialization
- Polling devices at their specified intervals
- Publishing changed values
- Routing received MQTT messages to appropriate devices


The Input_Base Class
---------------------

Location: ``lib/node_types/esp/src/dev_input_base.h`` and ``dev_input_base.cpp``

``Input_Base`` extends ``Device`` to provide functionality common to all input devices (buttons, analog sensors, etc.).

Key Features
~~~~~~~~~~~~

1. **Pin Management**
   
   - Stores the GPIO pin number
   - Supports multiplexer (mux) configurations for expanding inputs

2. **Precision Buffer**
   
   - Can buffer multiple readings for statistical analysis
   - Useful for filtering noisy sensor data
   - Configurable via ``precise_buffer(interval_ms, reads)``

3. **Reading Interface**
   
   - Abstract ``read()`` method implemented by derived classes
   - ``measure()`` method calls ``read()`` and stores result

Example Usage in Inheritance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   class Input_Base : public Device {
   protected:
       int _pin;
       int* buffer;
       int buffer_fill;
       
   public:
       Input_Base(const char* name, int pin, unsigned long pollrate_us = 10000);
       
       // Virtual method to be implemented by derived classes
       virtual int read() { return -1; }
       
       // Buffer management
       Input_Base& precise_buffer(unsigned long interval_ms, unsigned int reads);
       int fill_buffer(int val);
       
       void start() override;
       bool measure() override;
   };

The Input class (digital input) extends Input_Base:

.. code-block:: cpp

   class Input : public Input_Base {
   private:
       const char* _high;
       const char* _low;
       bool _inverted;
       bool _pull_up = true;
       
   public:
       Input(const char* name, int pin, 
             const char* high=str_on, const char* low=str_off, 
             bool inverted = false);
       
       int read() override { 
           return fill_buffer(digitalRead(_pin)); 
       }
       
       Input& with_pull_up(bool pull_up=true);
       Input& debounce(int threshold);
   };


The I2C_Device Class
--------------------

Location: ``lib/node_types/esp/src/i2c-device.h`` and ``i2c-device.cpp``

``I2C_Device`` extends ``Device`` for sensors that communicate via the I2C protocol.

Key Features
~~~~~~~~~~~~

1. **I2C Bus Management**
   
   - Manages SDA and SCL pins
   - Configurable clock speed
   - Support for multiple I2C buses (ESP32)

2. **Device Addressing**
   
   - Client address configuration
   - Master address for two-way communication

3. **Bus Health**
   
   - ``scan()`` method to detect if device is connected
   - ``clear_bus()`` to recover from bus lockup

4. **Measurement Wrapper**
   
   - ``measure_init()`` and ``measure_exit()`` wrap measurements
   - Ensures I2C bus is properly configured for each measurement

Lifecycle Customization
~~~~~~~~~~~~~~~~~~~~~~~

I2C devices override ``i2c_start()`` instead of ``start()``:

.. code-block:: cpp

   class I2C_Device : public Device {
   private:
       TwoWire mywire;
       uint8_t sda_pin;
       uint8_t scl_pin;
       uint8_t _i2c_address;
       
       void start() override; // Handles I2C initialization
       
   public:
       I2C_Device(const char* name, uint8_t client_address);
       
       // I2C configuration
       I2C_Device& i2c(uint8_t sda, uint8_t scl, unsigned int clock = 0);
       I2C_Device& set_address(uint8_t client_address);
       
       // Device detection
       bool scan();
       
       // Derived classes override this instead of start()
       virtual void i2c_start() { _started = true; }
       
       // Measurement helpers
       void measure_init();
       void measure_exit();
   };

Example I2C Device Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The BMP180 barometer sensor shows a typical I2C device implementation:

.. code-block:: cpp

   class Barometer_BMP180 : public I2C_Device {
   private:
       BMP085 *sensor = NULL;
       
   public:
       Barometer_BMP180(const char* name);
       
       void i2c_start() override {
           measure_init();
           sensor = new BMP085();
           if (sensor && sensor->begin()) {
               _started = true;
               add_subdevice(new Subdevice(F("temperature")));
               add_subdevice(new Subdevice(F("pressure")));
           }
           measure_exit();
       }
       
       bool measure() override {
           if (!started()) return false;
           measure_init();
           float temp = sensor->readTemperature();
           float pressure = sensor->readPressure();
           value(0).from(temp, 1);  // temperature with 1 decimal
           value(1).from(pressure, 0);  // pressure as integer
           measure_exit();
           return true;
       }
   };


The Output Class
----------------

Location: ``lib/node_types/esp/src/dev_output.h`` and ``dev_output.cpp``

``Output`` extends ``Device`` directly for digital output control (LEDs, relays, etc.).

Key Features
~~~~~~~~~~~~

1. **State Management**
   
   - High/low state control
   - Configurable command strings (e.g., "on"/"off")
   - Inversion support for active-low outputs

2. **Command Interface**
   
   - ``high()``/``on()`` methods to turn output on
   - ``low()``/``off()`` methods to turn output off
   - ``toggle()`` to switch state
   - ``set(value)`` to set by string command

3. **MQTT Integration**
   
   - Automatically subscribes to command topic
   - Publishes state changes
   - Supports Home Assistant discovery

Example
~~~~~~~

.. code-block:: cpp

   class Output : public Device {
   private:
       const char* _high;
       const char* _low;
       bool _inverted;
       int _pin;
       
   public:
       Output(const char* name, const int pin, 
              const char* high_command=str_on,
              const char* low_command=str_off,
              bool inverted=false);
       
       void start() override {
           pinMode(_pin, OUTPUT);
           _started = true;
       }
       
       Output& high() {
           if(started()) digitalWrite(_pin, _inverted?0:1);
           value().from(_high);
           return *this;
       }
       
       Output& low() {
           if(started()) digitalWrite(_pin, _inverted?1:0); 
           value().from(_low);
           return *this;
       }
   };


Device Manager
--------------

Location: ``lib/node_types/esp/src/device-manager.h`` and ``device-manager.cpp``

The ``DeviceManager`` is a singleton that manages all devices in a node.

Responsibilities
~~~~~~~~~~~~~~~~

1. **Device Registration**
   
   - Maintains a list of all devices
   - Automatically registers devices during construction

2. **Lifecycle Management**
   
   - Calls ``start()`` on all devices during initialization
   - Tracks which devices started successfully

3. **Polling**
   
   - Calls ``measure()`` on devices at appropriate intervals
   - Manages precision polling for devices that need it

4. **Publishing**
   
   - Publishes changed values for all devices
   - Handles MQTT communication

5. **Command Routing**
   
   - Receives MQTT messages
   - Routes commands to appropriate devices


Callbacks and Filters
----------------------

Devices support callback chains for advanced functionality:

Filter Callbacks
~~~~~~~~~~~~~~~~

Filter callbacks can process or validate values before publishing:

.. code-block:: cpp

   // Example: Only publish if value changed by at least 0.5
   device.with_filter_callback(*new Callback([](Device& dev) {
       float current = dev.value().as_float();
       float last = dev.get_last_confirmed_value().as_float();
       return abs(current - last) >= 0.5;
   }));

On-Change Callbacks
~~~~~~~~~~~~~~~~~~~

Callbacks triggered when values change:

.. code-block:: cpp

   // Example: Turn on LED when button is pressed
   button.with_on_change_callback(*new Callback([](Device& dev) {
       if (dev.is("on")) {
           led.on();
       }
       return true;
   }));


Creating New Device Types
--------------------------

To create a new device type:

1. **Choose Base Class**
   
   - Use ``Device`` for simple devices
   - Use ``Input_Base`` for input sensors
   - Use ``I2C_Device`` for I2C sensors

2. **Create Header File**
   
   Create ``lib/node_types/esp/src/dev_mydevice.h``:
   
   .. code-block:: cpp
   
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

3. **Create Implementation File**
   
   Create ``lib/node_types/esp/src/dev_mydevice.cpp``

4. **Register in devices.ini**
   
   Add entry to ``lib/node_types/esp/src/devices.ini``:
   
   .. code-block:: ini
   
      [mydevice]
      aliases = mydev
      filename = mydevice
      lib = some/library@^1.0.0

5. **Use in setup.cpp**
   
   The device is now available:
   
   .. code-block:: cpp
   
      mydevice("sensor1", D1);


Summary
-------

The IoTempower device architecture provides:

- **Consistent Interface**: All devices follow the same lifecycle and communication patterns
- **Easy Extension**: New device types can be created by inheriting from appropriate base classes
- **Automatic Management**: The DeviceManager handles registration, polling, and publishing
- **Flexible Configuration**: Callbacks and filters enable custom behavior
- **MQTT Integration**: Built-in support for publishing and receiving MQTT messages

This architecture enables rapid prototyping and deployment of IoT systems while maintaining code quality and consistency.


Related Documentation
---------------------

- `Architecture Overview <architecture.rst>`_ - Complete system architecture
- `Deployment Process <deployment-process.rst>`_ - How device code is compiled and deployed
- `Architecture Quick Reference <architecture-quick-reference.rst>`_ - Quick reference guide
- `Command Reference <node_help/commands.rst>`_ - Available device types
