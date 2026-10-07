dsm501a
========

..  code-block:: cpp

    dsm501a(name, pm1_pin, pm25_pin);

Create a new DSM501A dust sensor device.

The sensor exposes two pulse outputs. The driver measures low-pulse occupancy
for a 30 second sample window and publishes estimated concentrations for both
channels as separate subtopics.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network.
  Inside the code it can be addressed via IN(name).

- ``pm1_pin``: where the DSM501A PM1 output pin is connected to

- ``pm25_pin``: where the DSM501A PM2.5 output pin is connected to

Outputs
-------

- ``pm1``: estimated PM1 concentration from the PM1 output.
- ``pm25``: estimated PM2.5 concentration from the PM2.5 output.

The driver uses a fixed 30 second measurement window, matching the common
DSM501A low-pulse occupancy calculation pattern.

Example
-------

..  code-block:: cpp

    dsm501a(dust, D6, D5);

In this example:

- ``D6`` reads the DSM501A ``PM1`` output
- ``D5`` reads the DSM501A ``PM2.5`` output
