=======
ulnoiot
=======

Introduction
------------

*ulnoiot* (pronounced: "You'll know IoT") is a framework and environment
for making the entrance into working with the Internet of Things (IoT) accessible
for everybody -- tinkerers, makers, programmers, hobbyists, students,
and professionals alike.
It has a special focus on education and is intended to support classes to teach
Internet of Things (IoT) and
home automation.

However, it also supports existing IoT deployments and brings
mechanisms for over the air (OTA) updates and automatic
multi-device deployment.

If you are impatient and want to dive into it right now, forward to
`Getting Started`_.

It is based on a multi-layered network architecture. This means for this project
that each IoT-system (small network of connected sensors and actors) has its own
gateway - usually running an MQTT-broker. These gateways can be connected to cloud
resources, other cloud or Internet based MQTT brokers or interconnected among
themselves. Security and privacy can be selectively controlled at each layer
border (everywhere, where a gateway connects to layers).


Supported Hardware
------------------

*ulnoiot* is targeted to run on a variety of (mainly Linux-based) hardware and
on wireless microcontrollers (initially mainly esp8266-based microcontrollers and
single-board Linux computers like the Raspberry Pi Zero W).

If you are interested in shopping for related hardware, check http://iot.ulno.net/hardware.

The server side has been tested to run on:

- Raspberry Pi 1 (B and B+), 2, 3, and Zero W
- Linux laptop running Ubuntu 17.04

We are trying to provide virtualbox images as soon as we find time or volunteers.

We are also working on verifying that ulniot works well on Orange-Pi Zero and
the C.H.I.P. from NextThing to allow cost-effective solutions to use ulnoiot.

The part of ulnoiot running on the esp8266 is an extension of
`micropython <http://www.micropython.org/>`__
enabling IoT classes and easy getting started using 
micropython.
To support this start, we are using our own development kits as well as 
supporting very simple
selections of devices usually including the ESP8266 board Wemos D1 mini.

1st supported development kit: ulnoiot_devkit1
++++++++++++++++++++++++++++++++++++++++++++++

For a start, the original author Ulno (http://ulno.net) has designed some
shields (little electronic parts you can just plug together) to plug into
a Wemos D1 mini. This is now called the devkit1.
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
  this is a shield using the mpr121 multi touch and gpio chip over i2c
- `relay </doc/shields/wemosd1mini/relay/README.md>`__:
  supporting the default relay shield for the wemos d1 mini attached to d1

Tool support
------------

*ulnoiot* includes a lot of small scripts to simplify a plethora of tasks
supporting the deployment of a small IoT system or
`MQTT <http://mqtt.org/>`__-based IoT environment.
Among them are the following (most of these scripts are available after starting
the ulnoiot command or executing run in the main ulnoiot directory):

- run/ulnoiot: enter or run a command in the ulnoiot-environment (setting
  system variables and virtual python environments)
- accesspoint: start an accesspoint on a free wifi interface
- console: connect to a serial or network connected esp8266 microcontroller,
  running micropython
- shell: starting mpfshell to connect to locally or network connected esp8266
  device
- install: (re-)install the ulnoiot environment
- download_firmware: download latest firmware
- flash_esp8266: flash the ulnoiot-modified micropython on a locally connected
  esp8266
- deploy_wemosd1mini: copy or update the modifieable files for the ulnoiot
  environment to a locally or remotely connected wemosd1mini

Getting Started
---------------

- setup ulnoiot: clone this repository

  - If you just want read-only access just type in a folder of your choice:
    ``git clone https://github.com/ulno/ulnoiot``

  - If you are a ulnoiot developer, use
    ``git clone git@github.com:ulno/ulnoiot``

- make ulnoiot runnable -> copy examples/scripts/ulnoiot into your bin folder and adapt
  the path

- install mosquitto (``sudo apt install mosquitto mosquitto-clients``)
  and eventually disable the server
  (``sudo systemctl stop mosquitto; sudo systemctl disable mosquitto``)

- start ulnoiot and agree and wait for dependencies to be downloaded
  (if packages are mssing, fix dependencies and try to run
  ``ulnoiot install clean``)

- After successfully entering ulnoiot (the prompt should have changed colors and
  show ulnoiot in red, white, and black), flash and deploy some hardware using
  for example ``flash_esp8266`` and ``deploy_wemosd1mini``.

- Access the command prompt with ``console`` (if only a wemos is connected
  the serial port will be dicovered automatically else supply it as usb1 or acm2
  or an IP address and password as paramaters). You can also use webrepl
  (see below) to access the wemos.

If something gets stuck, try to power cycle the wemos d1 mini.


To setup wifi on the esp8266 device,
connect to the wireless network of the esp8266,
when in access point mode and use 
`webrepl <http://micropython.org/webrepl/>`__ or your own local copy of it
from https://github.com/micropython/webrepl to get a command line.
You can also connect to the command line via a serial terminal
(use console in ulnoiot).
 
The webrepl password is hardcoded to "ulnoiot".
If you want to change the webrepl login password from default ulnoiot to
something else use ``import webrepl_setup``
Attention, the password can't be too complex,
so stick with something around 8 characters and not too many 
special things in it. I haven't figured out why, it seems to be a
webrepl thing - or just stick with the default.

Try typing help and check the small manual.
You can setup the wifi with ``wifi( "network-name", "password" )``.
You can scan
the existing wifi networks with ``wscan`` and when the wifi is configured,
you can see the current ip with typing wip.

If you create a user.py file
with your own context, this will be started automatically,
when the system boots. Try ``help("user.py")`` at the repl prompt.


External Resources
------------------

This project would not have been possible without a thriving open source
community around the Internet of Things. We make a lot of use of the following
tools:

- `mpfshell <https://github.com/wendlers/mpfshell>`__ forked for ulnoiot
  `here <https://github.com/ulno/mpfshell>`__.
- `create_ap <https://github.com/oblique/create_ap>`__ forked for ulnoiot
  `here <https://github.com/ulno/create_ap>`__.
- `mosquitto <https://mosquitto.org/>`__.
- `micropython reference  <https://micropython.org/>`__

As ulnoiot relies heavily on MQTT, it integrates very easily with
`home-assistant <http://home-assistant.io>`__ and
`openhab <https://openhab.org>`__.



Further Documentation
---------------------

- `micropython reference
  <https://docs.micropython.org/en/latest/esp8266/esp8266/quickref.html>`__.
- For a very light introduction in general python, take a look at
  `this <https://docs.python.org/3/tutorial/introduction.html>`__.
