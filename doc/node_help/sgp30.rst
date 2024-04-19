Grove SGP30 gas sensor
======================

..  code-block:: cpp

    sgp30(name)[.i2c(sda,scl)][.with_address(i2c_address)];

Create a new gas sensor device.
Name will be appended to assigned mqtt topic.
Corresponding values are published there as subtopics at a fixed **1 Hz** rate: 

- ``tvoc``, total volatile organice compounds - unit: **part per billion (ppb)** 
 
- ``co2eq``, carbon dioxide equivalent - unit: **part per million (ppm)**

**Note:**
The SGP30 sensor has a warm-up time of ~15 seconds after power-on.
The SGP30 sensor has an internal, timed self-calibration function.


Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``.i2c(sda,scl)``: move i2c to given ports - if not specified uses default 
  i2c ports. For the **M5 Stick C**, please use ``.i2c(32, 33)``.

- ``.with_address(i2c_address)``: the i2c_address is the address of the gas sensor.
  The default address for the SGP30 sensor types 0x58, make sure you select on
  the hardware a diffrent address if you would like to drive two or more Grove SGP30 sensors on the
  same i2c bus.

With ``.with_filter`` a filter function can be specified: (see uhelp filter).


Example
-------

Node name: ``room/gas``:

..  code-block:: cpp

    sgp30(gas); // uses default i2c pins: D2 (sda), D1 (scl) (Wemos D1 Mini ESP8266)

Now TVOC (in ppb), CO2eq (in ppm) are published as ``room/gas/tvoc``, ``room/gas/co2eq``.


**Attention:**
If using **M5 Stick C**, please add ``.i2c(32, 33)`` to override default i2c pins:

..  code-block:: cpp

    sgp30(gas).i2c(32, 33); // override default i2c pins: 32 (sda), 33 (scl) (M5 Stick C ESP32 Pico)


Wiring
------
The Grove SGP30 sensor uses the Grove i2c cable.
More about the `Grove ecosystem <https://wiki.seeedstudio.com/Grove_System/>`__.

Grove connector pinout:

.. code-block::
   GND connect this module to the system ground
   VCC you can use 5V or 3.3V for this module
   SDA i2c serial data (sda)
   SCL i2c serial clock (scl)


Resources
---------

`Seeed Studio Grove VOC and eCO2 Gas Sensor WiKi <https://wiki.seeedstudio.com/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/>`__
`Sensirion SGP30 Datasheet <https://sensirion.com/media/documents/984E0DD5/61644B8B/Sensirion_Gas_Sensors_Datasheet_SGP30.pdf/>`__
`Sensirion SGP30 Product Page <https://sensirion.com/products/catalog/SGP30/>`__