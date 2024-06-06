Sensirion ENS16X CO2 sensor
===========================

..  code-block:: cpp

    ens16x(name)[.i2c(sda,scl)][.with_address(i2c_address)][.with_filter(filter)];

Create a new ens16x co2 sensor device.

Name will be appended to assigned mqtt topic.

Corresponding values are published there as subtopics at a fixed **0.5 Hz** (2s) rate: 
 

- ``aqi``, air quality index - unit: **index** | range: 0 to 5


- ``co2eq``, carbon dioxide equivalent - unit: **part per million (ppm)** | range: 400 to 65000 ppm


- ``tvoc``, total volatile organic compounds - unit: **part per billion (ppb)** | range: 0 to 65000 ppb

**Features:**

- ...


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
    - The default address for the ens16x sensor is ``0x58``. 
    - Select a different address if you would like to drive multiple sensors on the same i2c bus.


- ``.with_filter`` 
    - You can specify a filter function.
    - See ``uhelp filter`` for more.


Example
-------

Node name: ``room/gas``:

..  code-block:: cpp

    ens16x(gas);

Now the node publishes AQI, CO2eq (ppm) and TVOC every 5 seconds as: 
``room/gas/aqi``, ``room/gas/co2eq``,  ``room/gas/tvoc``.


**Note:** If using **M5StickC**, please add ``.i2c(32, 33)`` to override default i2c pins:

..  code-block:: cpp

    ens16x(gas).i2c(32, 33);


Wiring
------

.. code-block::

   GND  connect this module to the system ground
   VDD  you can use 2.4V up to 5V for this module
   SDA  i2c serial data (sda)
   SCL  i2c serial clock (scl)


Resources
---------

Sciosense ENS16X Datasheet:

    ENS160: https://www.sciosense.com/wp-content/uploads/2023/12/ENS160-Datasheet.pdf

DFRobot_ENS16X Library:
    https://github.com/DFRobot/DFRobot_ENS160/