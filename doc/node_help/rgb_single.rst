rgb_single
==========

..  code-block:: cpp

    rgb_single(name, pin_r, pin_g, pin_b, /* invert */ false);

Create a new rgb device object for an rgb-led connected to the pins
``pin_r`` (red pin), ``pin_g`` (green pin), and ``pin_b`` (blue pin).
Name will be appended to the mqtt topic.
If invert parameter is set to true, the outputs are inverted, for example
``255,0,127`` becomes ``0,255,128``.

Topics
------

Under the resulting topic will be the following subtopics:

- ``set``: turn on or off

- ``brightness/status``, ``brightness/set``: read and set brightness (0-255)

- ``rgb/status``, ``rgb/set``: read and set color as colorname, 6-digit hexcode (rrggbb)
  or comma separated triplet (r,g,b)

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``pin_r``, ``pin_g``, ``pin_b``: the gpiopin for red, green, and blue lines

- ``ignore_case``: should case in the command detection be ignored


Example
-------

**node name:** ``living room/tvlights``

..  code-block:: cpp

    rgb_single(rgb1, D3, D4, D2);


Now the RGB LED can be switched to red via sending to the mqtt-broker
to the topic ``living room/tvlights/rgb1/rgb/set`` the command ``red``.

Wiring
------

If you have an integrated single RGB module with pins, there will be resitors
on the module, and you can wire the module directly to the GPIO ports
(RGB to the diffrent ports) and ground (or -) to ground.

If you want to wire up a single RGB led (Comman Cathode) with four legs,
soldered or on a breadboard, you need 3 resistors (100-500 Ohm range)
in front of the R, G, and B channels. Check these resrouces to get an ideally
about the wiring:

- http://www.learningaboutelectronics.com/Articles/Common-cathode-RGB-LED-circuit.php (last visited 2021-11-10)

- https://create.arduino.cc/projecthub/muhammad-aqib/arduino-rgb-led-tutorial-fc003e (last visited 2021-11-10)

- https://randomnerdtutorials.com/esp8266-rgb-color-picker/ (last visited 2021-11-10)