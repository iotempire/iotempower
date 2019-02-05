barometer
=========

..  code-block:: cpp

    bmp085(name)[.i2c(sda,scl)][.with_address(i2c_address)];
    bmp180(name)[.i2c(sda,scl)][.with_address(i2c_address)];
    bmp280(name)[.i2c(sda,scl)][.with_address(i2c_address)];

Create a new barometer sensor device.
name will be appended to the mqtt topic and corresponding
values will be published there under the subtopics temperature, pressure,
and altitude.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``.i2c(sda,scl)``: move i2c to given ports - if not specified uses default 
  i2c ports

- ``.with_address(i2c_address)``: the i2c_address is the address of the barometer.
  The default address is for all barometer types 0x77, make sure you select on
  the hardware a diffrent address if you want ot drive two barometers on the
  same i2c bus.

With ``.with_filter`` a filter function can be specified: (see uhelp filter).

Example
-------

node name: ``living room/bm``:

..  code-block:: cpp

    bmp280(bmp1);

Now temperature (in Celsius), pressure (in Hektopascal), and altitude (in m)
are published as ``living room/bm/bmp1/temperature``,
``living room/bm/bmp1/pressure``, and ``living room/bm/bmp1/altitude``.