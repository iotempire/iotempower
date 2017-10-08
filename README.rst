=======
ulnoiot
=======

Introduction
------------

*ulnoiot* (pronounced: "You'll know IoT") is a framework and environment
for making it easy for everyone to explore and develop for the
Internet of Things (IoT)
-- easy for tinkerers, makers, programmers, hobbyists, students,
and professionals alike.
It has a special focus on education and is intended to support classes to teach
Internet of Things (IoT) and
home automation.

However, it also supports existing IoT deployments and brings
mechanisms for over the air (OTA) updates and automatic
multi-device deployment.

If you are impatient and want to dive into it right now, forward to
`Installation`_ or `First IoT Nodes`_.

It is based on a multi-layered network architecture. This means for this project
that each IoT-system (small network of connected sensors and actors) has its own
gateway - usually running an MQTT-broker. These gateways can be connected to cloud
resources, other cloud or Internet based MQTT brokers or interconnected among
themselves. Security and privacy can be selectively controlled at each layer
border (everywhere, where a gateway connects two layers).


Supported Hardware
------------------

*ulnoiot* is targeted to run on a variety of (mainly Linux-based) hardware and
on wireless microcontrollers (initially mainly esp8266-based microcontrollers and
single-board Linux computers like the Raspberry Pi Zero W).

If you are interested in shopping for related hardware, check http://iot.ulno.net/hardware.

The server side has been tested to run on:

- Raspberry Pi 1 (B and B+), 2, 3, and Zero W
- Linux laptop running Ubuntu 17.04

We are trying to provide virtualbox images as soon as we find time and/or volunteers.

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
  system variables and virtual python environments). To run a command, prefix
  it by the exec keyword, i.e.: ``ulnoiot exec accesspoint``

- accesspoint: start an accesspoint on a free wifi interface

- download_firmware: download latest firmware

- shell: starting mpfshell to connect to locally or network connected esp8266
  device

- install: (re-)install the ulnoiot environment (if you specify clean,
  it re-installs)

- network based commands:

    - console: (determines destiantion via the directory you are in) open a
      console to the respective node (the one configured with the local
      directory)

    - flash: firmware update over the network of the currently selected
      configuration folder (or all it's configuration sub-folders)

    - update: update ulnoiot micropython user-mode (non firmware) extensions
      over the network of the currently selected
      configuration folder (or all it's configuration sub-folders)

    - deploy: update ulnoiot micropython node coniguration (non firmware) extensions
      over the network of the currently selected
      configuration folder (or all it's configuration sub-folders).
      If noupdate is specified as option, only update the node-specific files.


- serial connection based commands:

    - console_serial: connect via serial to a locally connected
      microcontroller
    - flash_serial_esp8266: flash the ulnoiot-modified micropython on a locally connected
      esp8266
    - update_serial_wemosd1mini: copy or update the modifieable files for the ulnoiot
      environment to a locally or remotely connected wemosd1mini

Installation
------------

- install dependencies:
  ``sudo apt install git mc mosquitto mosquitto-clients virtualenv iptables bridge-utils``

- disable the mosquitto server:
  ``sudo systemctl stop mosquitto; sudo systemctl disable mosquitto``

- setup ulnoiot: clone this repository

  - If you just want read-only access just type in a folder of your choice:
    ``git clone https://github.com/ulno/ulnoiot``

  - If you are a ulnoiot developer, use
    ``git clone git@github.com:ulno/ulnoiot``

- make ulnoiot runnable -> copy examples/scripts/ulnoiot into your bin folder and adapt
  the path in it to reflect the location where you cloned ulnoiot. If you use
  tmux or byobu with bash consider sourcing ``lib/shell_starter/ulnoiot.bash``
  in your .bashrc.

- start ulnoiot and agree and wait for dependencies to be downloaded
  (if packages are mssing, fix dependencies and try to run
  ``ulnoiot install clean``)

- After successfully entering ulnoiot (the prompt should have changed colors and
  show ulnoiot in red, white, and black), start configuring your first IoT node,
  see `First IoT Nodes`_

First IoT Nodes
---------------

- Copy the folder ``lib/system_templates`` to a project directory,
  you can rename
  system_templates to a project name (i.e. iot-test-project)

- Rename the included node_template to a name for the node you want to
  configure (i.e. onboard_blinker)

- Adapt and configure system.conf and node.conf. If you installed ulnoiot on
  an orange pi or raspberry pi, you might want to also configure etc/ulnoiot.conf
  and run ``accesspoint`` and ``mqtt_broker``

- now change into your node directory, connect an esp8266 based microcontroller
  to your pc or raspberry/orange pi and type ``initialize``. This flashes and
  pre-configures the device.

- Access the command prompt with ``console_serial`` (if only one esp is connected
  the serial port will be discovered automatically else supply it as usb1 or acm2
  or an IP address and password as paramaters). If your wifi network is
  configured correctly, you can just type console. Its ip should now 
  automatically be discovered, and you get a web-based terminal on it.

If something gets stuck, try to power cycle the esp8266.

``initialize`` sets up your wifi based on the settings in system.conf and also
encrypts the network connecting, but if you want to set it up manually,
call ``wifi`` on the esp8266 node from the serial console.

Try typing ``help`` and check the small manual.
You can setup the wifi with ``wifi( "network-name", "password" )``.
You can scan
the existing wifi networks with ``wscan`` and when the wifi is configured,
you can see the current ip with typing wip.

If you create an autostart.py file or modify the existing one in your
nodes/files directory and then call ``deploy noupdate``, you can add your own
devices to this newly configured node. Don't forget to add ``run()`` add the end
of your autostart file.
However, try first to add some devices manually at the console comamnd prompt,
check and browse the help for available devices. type ``run()`` to activate
these devices and then use the ``mqtt_all`` and ``mqtt_send`` tools to watch and
interact.
Try also ``help("autostart.py")`` at the console prompt.



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
