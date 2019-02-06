hcsr04/rcwl-1601
================

..  code-block:: cpp

    hcsr04(name, trigger_pin, echo_pin, timeout_us=30000);

Create a new HC-SR04 (or RCWL-1601) ultrasonic distance measuring device.
It reads the distance in mm.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``trigger_pin``: where the trigger pin is connected to

- ``echo_pin``: where the echo pin is connected to

- ``timeout_us``: how long to max wait for a return signal (usually no need to 
  change default)

- ``.with_precision(precision)``: can be used to only react for changes in
  measuring that are bigger or equal to this value

**WARNING:**
The HCSR04 usually needs 5V input voltage (some though also work with 3.3V,
test first on 3.3V, if it doesn't work build teh voltage divider and
try with 5V). To read the Echo Pin with a 3.3V
device like the esp8266 or a Raspberry Pi zero, you must use a level shifter 
or setup a voltage divider for the echo-pin (for example echo pin via 1kOhm to 
the GPIO-Echo-Input and 2kOhm to ground).

The RCWL-1601 is specified for 3.3V and does not need the voltage
divider, do not connect it to 5V!

Example
-------

..  code-block:: cpp

    hcsr04(distance, D2, D1).with_precision(10);
