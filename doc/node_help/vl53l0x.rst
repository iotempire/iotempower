vl53l0x
=======

..  code-block:: cpp

    vl53l0x(name/*, long_range is by default false*/ /*, high_accuracy_slow is by default false*/); /* replace one full comment with , true or , false if you want to change the defaults. */

Create a new VL53L0X laser time-of-flight distance measuring device.
It reads the distance in mm using laser technology with high precision.

The VL53L0X is an I2C sensor that provides accurate distance measurements
up to about 2 meters (8190mm maximum). It offers two modes for different
use cases: long range mode for extended distance measurement with lower
accuracy, and high accuracy slow mode for precise measurements at the
cost of slower measurement speed.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``long_range``: optional boolean (default false). When true, enables long range
  mode which extends the maximum measurement distance but reduces accuracy and
  measurement speed.

- ``high_accuracy_slow``: optional boolean (default false). When true, enables
  high accuracy mode which provides more precise measurements but takes longer
  (200ms vs 20ms timing budget).

- ``.with_precision(precision)``: can be used to only react for changes in
  measuring that are bigger or equal to this value

- ``.precision(precision)``: alias for with_precision()

- ``.filter_binarize(threshold, "high_word", "low_word")``: convert distance readings
  to binary output based on threshold distance

- ``filter_function``: specify a preprocessor for measured values
  (see `filter <filter.rst>`_)

Example
-------

..  code-block:: cpp

    vl53l0x(distance);

..  code-block:: cpp

    vl53l0x(distance, true, false).with_precision(5);

..  code-block:: cpp

    vl53l0x(long_distance, true, true);

..  code-block:: cpp

    vl53l0x(proximity, false, false)
        .filter_binarize(500, "close", "far");

The first example creates a standard VL53L0X sensor with default settings.
The second example enables long range mode and sets precision to 5mm.
The third example enables both long range and high accuracy modes for
maximum range with best possible precision.
The fourth example uses the sensor as a proximity detector, sending "close"
when objects are within 500mm and "far" when beyond that distance.

Wiring
------

The VL53L0X is an I2C device that operates at 3.3V:

- **VCC**: Connect to 3.3V (do not use 5V)
- **GND**: Connect to ground
- **SDA**: Connect to the SDA pin of your microcontroller (usually D2 on Wemos D1 Mini)
- **SCL**: Connect to the SCL pin of your microcontroller (usually D1 on Wemos D1 Mini)

The sensor uses I2C address 0x29 (0b0101001) by default.

**Note**: The VL53L0X works reliably with 3.3V systems like ESP8266 and ESP32.
No level shifting is required.

Technical Details
-----------------

- **Measurement Range**: 30mm to 8190mm (typical use up to 2000mm)
- **Interface**: I2C (address 0x29)
- **Measurement Technology**: Time-of-Flight (ToF) laser ranging
- **Supply Voltage**: 2.6V to 3.5V (3.3V recommended)
- **Measurement Speed**: 20ms (default) or 200ms (high accuracy mode)
- **Accuracy**: ±3% (typical), better in high accuracy mode
- **Field of View**: 27 degrees

The sensor automatically handles ambient light compensation and provides
consistent measurements regardless of the target color or surface type.