soil_7in1
=========

..  code-block:: cpp

    soil_7in1(name, rx_pin, tx_pin /*, baud is by default 4800*/
        /*, config is by default SWSERIAL_8N1*/
        /*, invert is by default false*/
        /*, addr is by default 0x01*/
        /*, dir_pin is by default -1 (auto-direction)*/
        /*, timeout_ms is by default 400*/
        /*, retries is by default 3*/
        /*, moisture is by default true*/
        /*, temperature is by default true*/
        /*, ec is by default true*/
        /*, ph is by default true*/
        /*, nitrogen is by default true*/
        /*, phosphorus is by default true*/
        /*, potassium is by default true*/
        /*, salinity is by default true*/
        /*, tds is by default true*/);

Create a new 7-in-1 soil sensor over RS485 Modbus (input registers).

The device reads the sensor registers 0x0000..0x0008 and publishes selected
values as separate subtopics.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network.
  Inside the code it can be addressed via IN(name).

- ``rx_pin``: GPIO pin for the software serial RX line.

- ``tx_pin``: GPIO pin for the software serial TX line.

- ``baud``: serial baud rate (default 4800).

- ``config``: serial data configuration (default ``SWSERIAL_8N1``).

- ``invert``: set true to invert serial levels (default false).

- ``addr``: Modbus device address (default ``0x01``).

- ``dir_pin``: optional RS485 direction pin for non-auto adapters (default -1).

- ``timeout_ms``: Modbus response timeout in milliseconds (default 400).

- ``retries``: number of Modbus retries (default 3).

Outputs
-------

You can enable or disable each output via boolean parameters:

- ``moisture`` -> ``moisture`` (%, one decimal)
- ``temperature`` -> ``temperature`` (C, one decimal)
- ``ec`` -> ``ec`` (uS/cm, one decimal)
- ``ph`` -> ``ph`` (one decimal)
- ``nitrogen`` -> ``nitrogen`` (mg/kg, integer)
- ``phosphorus`` -> ``phosphorus`` (mg/kg, integer)
- ``potassium`` -> ``potassium`` (mg/kg, integer)
- ``salinity`` -> ``salinity`` (ppm, integer)
- ``tds`` -> ``tds`` (ppm, integer)

Examples
--------

Default (all fields):

..  code-block:: cpp

    soil_7in1(soil, D6, D7);

Only moisture and temperature:

..  code-block:: cpp

    soil_7in1(soil, D6, D7, 4800, SWSERIAL_8N1, false, 0x01, -1, 400, 3,
        true, true, false, false, false, false, false, false, false);
