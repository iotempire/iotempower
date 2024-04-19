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

- The sgp30 sensor has a warm-up time of ~15 seconds after power-on.

- The sgp30 sensor has an internal, timed self-calibration function.


Parameters
----------

- ``name``
    - name of topic can be addressed as via MQTT in the network. 
    - Inside the code it can be addressed via ``IN(name)``.


- ``.i2c(sda,scl)``
    - override i2c default ports - if not specified uses default i2c ports.
    - For the **M5StickC**, please use ``.i2c(32, 33)``.


- ``.with_address(i2c_address)``
    - ``i2c_address`` is the i2c address of the gas sensor.
    - The default address for the sgp30 sensor is ``0x58``. 
    - Select a different address if you would like to drive multiple sensors on the same i2c bus.


- ``.with_filter`` 
    - A filter function can be specified. 
    - See ``uhelp filter`` for more.


Example
-------

Node name: ``room/gas``:

..  code-block:: cpp

    sgp30(gas);

Now TVOC (in ppb), CO2eq (in ppm) are published as: ``room/gas/tvoc``, ``room/gas/co2eq``.



**Note:** If using **M5StickC**, please add ``.i2c(32, 33)`` to override default i2c pins:

..  code-block:: cpp

    sgp30(gas).i2c(32, 33);


Wiring
------

The Grove sgp30 sensor uses the Grove i2c cable.

More about the `Grove ecosystem <https://wiki.seeedstudio.com/Grove_System/>`__ here.

Grove connector pinout:


.. code-block::

   GND  connect this module to the system ground
   VCC  you can use 5V or 3.3V for this module
   SDA  i2c serial data (sda)
   SCL  i2c serial clock (scl)


Resources
---------

- `Seeed Studio Grove VOC and eCO2 Gas Sensor WiKi <https://wiki.seeedstudio.com/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/>`_

- `Sensirion sgp30 Datasheet <https://sensirion.com/media/documents/984E0DD5/61644B8B/Sensirion_Gas_Sensors_Datasheet_SGP30.pdf/>`_

- `Sensirion sgp30 Product Page <https://sensirion.com/products/catalog/SGP30/>`_