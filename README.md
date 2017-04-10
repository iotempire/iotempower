# ulnoiot

Here, your find extensions to [micropython](http://www.micropython.org/)
enabling IoT classes and easy getting started using 
[micropython](http://www.micropython.org/).
To support this start, we are using our own development kits as well as 
supporting very simple
selections of devices usually including one ESP8266 board.

## 1st supported development kit: ulno's devkit1
For a start, the original author Ulno (http://ulno.net) has desigened some
shields (little electronic parts you can just plug together) to plug into
a Wemos D1 mini. This is now called the devkit1.
You can also find manuals to build different parts of the 
devkit and the links to youtube videos using and soldering them on ulno's
[youtube channel](https://www.youtube.com/channel/UCaDpsG87Q99Ja2q3UoiXRVA).

devkit1 envisions the following shields:
- [devel](shields/devel/README.md): a shield with two leds and 3 buttons, pressing the right one when booting up, will
  delete the wifi configuration file and prevent the user_boot.py to be started
- [display](shields/display/README.md): supports using an i2c-based 128x64 LCD (ssd1306) on d5 and d2
- [ht](shields/ht/README.md): a shield employing a DHT11 temperature and humidity sensor (connected to d1)
- [mpr121](shields/mpr121/README.md): this is a shield using the mpr121 multi touch and gpio chip over i2c
- [relay](shields/relay/README.md): supporting the default relay shield for the wemos d1 mini attached to d1

## Tool support
Other tools allow an easy way to deploy the Micropython firmware
and also adding the present libraries to allow writing really simple
software for these devices and integrate them in an mqtt-based IoT environment.



To setup wifi connect to the wireless network of the esp8266,
when in access point mode and use 
[webrepl](http://micropython.org/webrepl/) or your own local copy of it 
from https://github.com/micropython/webrepl to get a command line.
You can also connect to the command line via a serial terminal.
 
The webrepl password is hardcoded to "ulnoiot". You can change it later in the 
[webrepl_cfg.py](/lib/webrepl_cfg.py) file (just re-upload the file with a different password and reconnect.
Attention, the password can't be too complex, 
so stick with something around 8 characters and not too many 
special things in it. I haven't figured out why, seems to be a 
webrepl thing - or just stick with the default.

Try typing help and chekc the small manual.
You can setup the wifi with wifi( "network-name", "password" ). You can scan
the existing wifi networks with wscan and when the wifi is configured, you can
see the current ip with typing wip.

If you create a user_boot.py file
with your own context, this will be started automatically,
when the system boots.

## External resources
This project would not have been possible without a thriving open source
community around the Internet of Things. We make a lot of use of the following
tools:
- [mpfshell](https://github.com/wendlers/mpfshell).
For further documentation, look here:
- [micropython reference](https://docs.micropython.org/en/latest/esp8266/esp8266/quickref.html).
- For a very light introduction in general python, take a look at 
[this](https://docs.python.org/3/tutorial/introduction.html).
