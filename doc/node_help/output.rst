output
======

..  code-block:: cpp

    output(name, pin, high_command="on", low_command="off");

**other names:** ``led``, ``out``, ``relais``, ``relay``

Create a new output port for a simple LED or a relay switch.
name will be appended to the mqtt topic and a set appended to
send commands to.

Parameters
----------

- name: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- pin: the gpio pin

- high_command: what needs to be sent to turn the respective port high

- low_command: what needs to be sent to turn the respective port low

Example
-------

**node name:** ``living room/leds1``

..  code-block:: cpp

    led(yellow, D7, "turn on", "turn off");

Now the LED can be switched on via sending to the mqtt-broker
to the topic ``living room/leds1/yellow/set`` the command
turn on

If you were looking for the driver for an RGB led, check 
`rgb_single <rgb_single.rst>`_ or `rgb_strip <rgb_strip>`_.