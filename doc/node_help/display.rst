display
=======

..  code-block:: cpp

  display(name, u8g2_display, font=font_medium)
    [.i2c(sda,scl)][.with_address(i2c_address)];
  display(name, font=font_medium)
    [.i2c(sda,scl)][.with_address(i2c_address)];
  display44780(name, width, height)
    [.i2c(sda,scl)][.with_address(i2c_address)];


Create a new controller for a ``_1_`` display supported by the u8g2 display library
(https://github.com/olikraus/u8g2) (``_1_`` is a specific mode to drive
the display).

Omitting the u82_display pre-selects a 128x64 ssd1306 lcd display connected via
I2C. Omitting scl and sda wil use the default hardware I2C bus.
With font, you can specify a text font defined in the u8gs library.
``display44780`` creates a controller for an I2C display driven by Hitachi's 
HD44780 chip (also called PCF8574T - for the I2C adapter - or 1602A orLCM1602).

``name`` will be appended to the mqtt topic and a set appended to
send text and commands to.

Parameters
==========

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``width``/``height``: dimensions of display in pixels
- ``.i2c(sda,scl)``: move i2c to given ports - if not specified uses default 
  i2c ports
- ``.with_address(i2c_address)``: the i2c_address is the address of the display.
  For the 44780 with the PCF8574T, it is usually 0x27 (default) but sometimes
  0x38. For the SSD1306 it is usually 0x78 (default) or 0x7a.
  If omitted, the default address is chosen.


Examples
========

..  code-block:: cpp

    // default ssd1306 128x64 i2c display
    display(dp1).i2c(D4, D3).with_address(0x78);

    // small Wemos D1 Mini SSD1306 64x48 display shield
    U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0); // R0 no rotation, R1 - 90°
    display(oled, u8g2, u8g2_font_profont29_mf);

    // Relatively large two line display in many sensor-kits
    display44780(display2, 16, 2);



Now the first display can be controlled via sending to the mqtt-broker
to the topic ``node_topic/dp1`` different text or commands (second one
would be controlled with topic ``node_topic/display2``).

Normal text will be printed as text - scrolling is supported.

There are also these special commands:

+--------------------------------+---------------------------------------------+
| ``&&clear`` or ``&&cl``        | to clear the display                        |
+--------------------------------+---------------------------------------------+
| ``&&goto x y`` or ``&&go x y`` | to set the text cursor (1 1 is upper left)  |
+--------------------------------+---------------------------------------------+
| ``&&nl`` or ``&&ln``           | to go to next line and evtl. scroll display |
+--------------------------------+---------------------------------------------+
