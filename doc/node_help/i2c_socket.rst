i2c_socket
==========

..  code-block:: cpp

    i2c_socket(name [plug_address, [socket_address]])[.i2c(sda,scl)][.pollrate(pr_ms)];

Create a new I2C connector socket device.

name will be appended to the mqtt topic and corresponding
values will be published there under the subtopics temperature, pressure,
and altitude.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network.
  Inside the code it can be addressed via IN(name).

- ``plug_address``: the I2C address, the plug is registered under (default 9).

- ``socket_address``: the I2C address to assume as the I2C master (default 8).

- ``.i2c(sda,scl)``: move i2c to given ports - if not specified uses default 
  i2c ports.


With ``.with_filter`` a filter function can be specified: (see uhelp filter).

Example
-------

node name: ``lab1/experiment2``:

..  code-block:: cpp

    i2c_socket(arduino1, 9, 8).pollrate(2000);

This creates an I2C connector socket device with address 8 that talks to
via I2C to a connected Arduino using the i2c-plug library. The plug has to uses
address 9. It receives values from the Arduino every 2 seconds (2000ms).
Attention, the I2C bus needs termination resistors to 3.3V, both on SCL and SCA.
Usually something in the range of 2k to 10k works well here. 

Under the MQTT topic ``lab1/experiment2/arduino1`` will now the values reported
that are read every two seconds from the Arduino plug driver.
On ``lab1/experiment2/arduino1/set``, commands can be send to the receive
callback defined on the Arduino.
