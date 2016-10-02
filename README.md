# micropython-extra-ulno
Ulno's (http://ulno.net) extensions to [micropython](http://www.micropython.org/)
enabling IoT classes and easy getting started using [micropython](http://www.micropython.org/).

The tools here make heavily use of [mpfshell](https://github.com/wendlers/mpfshell).

The tools allow an easy way to deploy the firmware and the small libraries supporting
the follwing shields:
- [devel](shields/devel/Readme.md): a shield with two leds and 3 buttons, pressing the right one when booting up, will
  delete the wifi configuration file and prevent the user_boot.py to be started
- display: supports using an i2c-based 128x64 LCD (ssd1306) on d5 and d2
- ht: a shield emplying a DHT11 temperature and humidity sensor (connected to d1)
- mpr121: this is a shield using the mpr121 multi touch and gpio chip over i2c
- relay: supporting the default relay shield for the wemos d1 mini attached to d1
