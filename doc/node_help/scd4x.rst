Sensirion SCD4X CO2 sensor
==========================

..  code-block:: cpp

    scd4x(name[, temp_comp[, altitude[, ambient_pressure]]] )[.i2c(sda,scl)][.with_address(i2c_address)][.with_filter(filter)];

Create a new scd4x co2 sensor device.

Name will be appended to assigned mqtt topic.

Corresponding values are published there as subtopics at a fixed **0.2 Hz** (5s) rate: 
 

- ``co2``, carbon dioxide - unit: **part per million (ppm)** | range: 0 to 40000 ppm


- ``temp``, environment temperature - unit: **Celsius (°C)** | range: -10 to 60 °C


- ``humidity``, relative humidity - unit: **relative humidity (%RH)** | range: 0 to 100 %RH


**CO2 Sensor Performance:**

Specification provided by manufacturer for default conditions (25 °C, 50 %RH, ambient pressure 1013 mbar, periodic measurement, 3.3 V supply voltage).


- Output measurement range of ``0 ppm to 40.000 ppm`` CO2 concentration.


- Measurement response time τ63% ``~60 seconds``.


- Measurement accuracy of ``±(50 ppm + 5%)`` between ``400 to 2000 ppm`` CO2 concentration.
    - Typical measurement repeatability ``±10 ppm``.


- Low power operation mode down to ``< 0.4 mA avg. @5 V``, 1 meas. / 5 minutes.


- Additional ``±(5 ppm + 0.5 %)`` of reading accuracy drift after ``5 years`` with automatic self-calibration enabled (default). 

Parameters
----------

- ``name``
    - name of topic can be addressed as via MQTT in the network. 
    - Inside the code it can be addressed via ``IN(name)``.


- ``temp_comp`` (optional; default = 4.0)
    - 𝑇𝑜𝑓𝑓𝑠𝑒𝑡_𝑎𝑐𝑡𝑢𝑎𝑙 = 𝑇𝑆𝐶𝐷4𝑥 − 𝑇𝑅𝑒𝑓𝑒𝑟𝑒𝑛𝑐𝑒 + 𝑇𝑜𝑓𝑓𝑠𝑒𝑡_ 𝑝𝑟𝑒𝑣𝑖𝑜𝑢𝑠
    - Temperature offset helps optimize Relative Humidity and Temperature measurements.
    - Temperature offset depends on the measurement mode, self-heating of nearby components, ambient temperature and air flow. 
    - Recommend determining offset when installed and running under typical operational conditions and in thermal equilibrium.
    - Inside the code it can be addressed via ``IN(temp_comp)``.


- ``altitude`` (optional; default = 57)
    - Typically, the sensor altitude is set once after device installation. 
    - The default sensor altitude value is set to 0 meters above sea level. 
    - Valid input values are between 0 to 3000 m. 
    - Inside the code it can be addressed via ``IN(altitude)``.

- ``ambient_pressure`` (optional; default = 0)
    - Current ambient pressure in pascals (Pa); set to ``0`` to skip applying it.
    - Use this to compensate CO2 readings when you know the local pressure.
    - Inside the code it can be addressed via ``IN(ambient_pressure)``.


- ``.i2c(sda,scl)``
    - override i2c default ports - if not specified uses default i2c ports.
    - For the **M5StickC**, please use ``.i2c(32, 33)``.


- ``.with_address(i2c_address)``
    - ``i2c_address`` is the i2c address of the gas sensor.
    - The default address for the scd4x sensor is ``0x62``. 
    - Select a different address if you would like to drive multiple sensors on the same i2c bus.


- ``.with_filter`` 
    - You can specify a filter function.
    - See ``uhelp filter`` for more.


Example
-------

Node name: ``room/gas``:

..  code-block:: cpp

    scd4x(gas, 5.0, 60, 101300); // name: room/gas, temp_comp: 5.0 [deg C], altitude: 60 [m], ambient_pressure: 101300 [Pa]


Now CO2 (in ppm), temperature (in deg C) and humidity (%RH) are published every 5 seconds as: 
``room/gas/co2ppm``, ``room/gas/temp``,  ``room/gas/humidity``.


**Note:** If using **M5StickC**, please add ``.i2c(32, 33)`` to override default i2c pins:

..  code-block:: cpp

    scd4x(gas, 5.0, 60, 101300).i2c(32, 33); //for the M5StickC


Wiring
------

.. code-block::

   GND  connect this module to the system ground
   VDD  you can use 2.4V up to 5V for this module
   SDA  i2c serial data (sda)
   SCL  i2c serial clock (scl)


Resources
---------

Sensirion SCD4X Product Pages (both work with this driver):
    
    SCD40: https://sensirion.com/products/catalog/SCD40/
    
    SCD41: https://sensirion.com/products/catalog/SCD41/

Sensirion SCD4X Datasheet: 
    https://sensirion.com/media/documents/48C4B7FB/64C134E7/Sensirion_SCD4x_Datasheet.pdf

DFRobot_SCD4X Library:
    https://github.com/DFRobot/DFRobot_SCD4X/
