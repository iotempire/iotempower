1st supported development kit: ulnoiot_devkit1
==============================================

For a start, the original author Ulno (http://ulno.net) has designed some
shields (little electronic parts you can just plug together) to plug into
a Wemos D1 Mini. This is now called the devkit1.
You can also find manuals to build different parts of the
devkit and the links to youtube videos using and soldering them on
`ulno's youtube channel <https://www.youtube.com/channel/UCaDpsG87Q99Ja2q3UoiXRVA>`__.

devkit1 envisions the following shields:

- `devel </doc/shields/wemosd1mini/devkit1/2led3but/README.md>`__:
  a shield with two leds and 3 buttons
- `display </doc/shields/wemosd1mini/devkit1/display/README.md>`__:
  supports using an i2c-based 128x64 LCD (ssd1306) on d5 and d2
- `ht </doc/shields/wemosd1mini/devkit1/ht/README.md>`__:
  a shield employing a DHT11 temperature and humidity sensor (connected to d1)
- `mpr121 </doc/shields/wemosd1mini/devkit1/mpr121/README.md>`__:
  this is a shield using the mpr121 multi touch and gpio chip over i2c (unfinished)
- `relay </doc/shields/wemosd1mini/relay/README.md>`__:
  supporting the default relay shield for the Wemos D1 Mini attached to d1

Development of this kit is currently stalled in favor of wiring small device boards
like provided in the 37-in-1 Arduino sensor kit directly to the Wemos D1 Mini or
NodeMCU development board.
