rgb_strip
=========

..  code-block:: cpp


    // new version
    rgb_strip_bus(name, num_leds, color_feature, control_method, data_pin);

    // Obsolete version
    rgb_strip(name, num_leds, Striptype, data_pin, 
        ...other FastLed parameters...);

Create a new rgb strip device object for an rgb-led strip connected to the
given ``data_pin`` (or ``data`` and ``clock_pin``) with ``num_leds`` rgb leds.
All led chips supported by FastLED (https://github.com/FastLED/FastLED) are
supported. Attention: FastLED seems to be broken since beginning of 2020 and
does not work using WiFi on esp8266 anymore. Short led strips might still work,
but long strips will show flickering wrong colors, and other synchronization issues.
Please use the new rgb_strip_bus that uses precise timing but can only be wired to
specific datapins (like only D4 on the Wemos D1 Mini or NodeMCU).
Check https://github.com/Makuna/NeoPixelBus for this advanced version.


Topics
------

Name will be appended to the mqtt topic.

Under the resulting topic will be the following subtopics:

- ``set``: turn on or off

- ``brightness``, ``brightness/set``: read and set brightness (0-255)

- ``rgb``, ``rgb/set``:
  read and set color as colorname, 6-digit hexcode
  ``rrggbb`` or comma separated triplet ``r,g,b``

  This also takes a number to just affect one led
  It also takes the words front and back, which will
  push the new color into strip (from the front or the
  back) and move all other led-colors by one.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network.
  Inside the code
  it can be addressed via IN(name).

- ``data_pin``: the data-pin the led strip is connected to

- ``num_led``: number of leds on strip

- ``...other FastLed parameters...``: data related to chip configuration (use
  same format as in pointy brackets in ``FastLED.addLeds<>``).

Example
-------

**node name:** ``living room/tvlights``

..  code-block:: cpp

    // Only runs stable on D4 on esp8266 based boards
    rgb_strip_bus(strip2, 10, F_GRB, NeoEsp8266Uart1800KbpsMethod, D4);

    // Same version for esp32 minikit or lolin S2 mini, you can use other pins than D4 on the esp32:
    // any port bellow 34 are supported, however check this link for further documentation
    https://github.com/Makuna/NeoPixelBus/wiki/ESP32-NeoMethods 
    rgb_strip_bus(leds, num_leds, F_BRG, NeoEsp32I2s0Ws2812xMethod, 16).report_change(false);    
    // You can also try NeoEsp32Rmt0Ws2812xMethod if you use big matrixes as
    // it is supposed to use less buffer space (but more interrupts)

    // old obsolete and probably instable version, will be inaccessible soon
    rgb_strip(strip2, 10, WS2811, D6, BRG); // <- do not use!

Now the second RGB LED can be switched to red via sending to the mqtt-broker
to the topic ``living room/tvlights/strip2/rgb/set`` the command ``2 red`` or
``front red``.

Check also `rgb_matrix <rgb_matrix.rst>`_ and `animator <animator.rst>`_.

Wiring
------

Remember that the WS2811 needs 12V (but can still be driven from the dataline in
of the esp8266), make sure it uses the same ground as the esp8266.
The WS2812 and WS2812B use 5V, but make sure you have a big enough power supply.
You can drive about 10 LEDs directly from a wemos d1 mini connected to a
USB 3 port (a usb 2 port does not have enough power, maybe 5 LEDs).
