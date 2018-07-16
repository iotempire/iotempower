=======
ulnoiot
=======

Attention: this is ulnoiot-tng (the next generation, not anymore based on micropython, now using platformio as a base to generate firmware and code)
Most of the following documention still refers to the original micropython based ulnoiot version, if you want to work with this development version contact ulno directly or make youself known in the forum.


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

If you are impatient and want to dive into it right now, fast forward to
`Installation`_ or `First IoT Nodes`_.

ulnoiot is based on a multi-layered network architecture. This means for this project
that each IoT-system (small network of connected sensors and actors) has its own
gateway - usually running an MQTT-broker. These gateways can be connected to cloud
resources, other cloud or Internet based MQTT brokers or interconnected among
themselves. Security and privacy can be selectively controlled at each layer
border (everywhere, where a gateway connects two layers).
In a teaching context these gateways are usally based on a modified Raspbian
running on a Raspberry Pi of a
newer generation (wifi on board - both the Raspberry Pi 3 and the Raspberry Pi Zero W
can be easily used as an ulnoiot gateway at this point)
to allow them to work as wifi routers.


Table of Contents
-----------------

- `Introduction`_

- `Architecture`_

- `Supported Hardware`_

- `Tool Support`_

- `Installation`_

- `First IoT Nodes`_

- `External Resources`_

- `Support`_

- `Further Documentation`_



Architecture
------------

.. figure:: /doc/images/system-architecture.png
   :width: 50%
   :figwidth: 100%
   :align: center
   :alt: ulnoiot System Architecture

   ulnoiot System Architecture

In an ulnoiot system you find one or several ulnoiot gateways and wirelessly
connected nodes with physically connected devices (sensors or actors).

An *ulnoiot gateway* contains and runs all the software needed to configure an
IoT system. It also provides facilities to run and manage a wifi router as well
as an MQTT-broker. It therefore provides configuration management software as
well as dataflow management services.

Such a gateway can for example easily be installed on a Raspberry Pi 3.
*Nodes* are wireless components which interact with physical obejcts. Usually a
*node* has several *devices* attached to it. *devices* can be sensors (like a button or
temperature, light, movement, or humidity sensor) or actors (like relais, solenoids,
motors, or leds).


Supported Hardware
------------------

*ulnoiot* is targeted to run on a variety of (mainly Linux-based) hardware and
on wireless microcontrollers (initially mainly esp8266-based microcontrollers and
single-board Linux computers like the Raspberry Pi 3 or Raspberry Pi Zero W).

If you are interested in shopping for related hardware, check http://iot.ulno.net/hardware
or go directly to AliExpress, Amazon, AdaFruit or Sparkfun and search for Wemos
D1 Mini, ESP8266, NodeMCU, 37-in-1 Arduino sensor kit.

The gateway services have been tested to run on:

- Raspberry Pi 1 (B and B+), 2, 3, and Zero W
- Linux laptop running Ubuntu 17.04

We are trying to provide virtualbox images as soon as we find time and/or volunteers.

We are also working on verifying that ulniot works well on Orange-Pi Zero and
the C.H.I.P. from NextThing to allow more cost-effective solutions to use ulnoiot.

Currently the following esp8266-based devices are supported:

- Wemos D1 Mini
- NodeMCU
- Espresso Lite V2
- Sonoff and Sonoff Touch
- There is an esp8266 generic opition for other esp8266-based boards.

The part of ulnoiot running on the esp8266 is an extension of
`micropython <http://www.micropython.org/>`__
enabling IoT classes and easily getting started using
micropython.

There has been some initial effort in creating a starter development kit for
the Wemos D1 Mini - you can see more information `here
</doc/shields/wemosd1mini/devkit1/README.rst>`__.



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

- ulnoiot upgrade: get latest version of ulnoiot (inside an existing version)

- shell: starting mpfshell to connect to locally or network connected esp8266
  device

- install: (re-)install the ulnoiot environment (if you specify clean,
  it re-installs)

- network based commands:

  - ``console``: (determines destination via the directory you are in) open a
    console to the respective node (the one configured with the local
    directory)

  - ``initialize``: initialize a current node including a serial flash

  - ``flash``: firmware update over the network (OTA) of the currently selected
    configuration folder (or all it's configuration sub-folders)

  - ``update``: update ulnoiot micropython user-mode (non firmware) extensions
    over the network of the currently selected
    configuration folder (or all it's configuration sub-folders)

  - ``deploy``: update ulnoiot micropython node coniguration (non firmware) extensions
    over the network of the currently selected
    configuration folder (or all it's configuration sub-folders).
    If noupdate is specified as option, only update the node-specific files.

- serial connection based commands:

  - ``console_serial``: connect via serial to a locally connected
    microcontroller

  - ``initialize``: set up (flash, update, and deploy) an ulnoiot node for the first time

  - ``flash_serial``: flash the ulnoiot-modified micropython on a locally connected
   esp8266

  - ``update_serial [alsodeploy]``: copy or update the modifieable files for the ulnoiot
    environment to a locally or remotely connected wemosd1mini


Installation
------------

There are two ways to get the ulnoiot configuration management software
up and running:

1. Downloading and flashing a pre-prepared raspberry pi image to an sd card
   and running the gateway and configuration management software from there.

2. Setting up ulnoiot in your own Linux environment:
   `Installation on Linux`_

Please also check out the tutorial videos for this setup on ulno's youtube
channel: https://www.youtube.com/results?search_query=ulno.net+ulnoiot


Installation on Raspberry Pi from Pre-Prepared Image
++++++++++++++++++++++++++++++++++++++++++++++++++++

Tutorial videos for setup:

- `On MacOS <https://www.youtube.com/watch?v=oHM-ojoST-c>`__

- `On PC/Windows <https://youtu.be/DZ-PwxIc8wY>`__

- On Linux: Please follow the installation steps below (or file an issue to ask
  for specific Linux video).

  

Installation step by step:

- Download the Raspberry Pi image from here: https://goo.gl/bVgLMr

- Make sure the sha256-checksum of the image is correct. It should be:

  bd0383eb82bcd9fcf6e6ca83c6b34a524af8e6225065a8576da592cb91ecf0e7

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to a (at least) 8GB class-10 sd-card with https://etcher.io/
  (works on Windows, MacOS, or Linux).

- Open the sd-card on your pc. You will see a file named config.txt
  Edit this file and scroll to the bottom. Change and uncomment the options
  ``uiot_ap_name`` and ``uiot_ap_password`` to your own values,
  remember the password
  you set in ``uiot_ap_password``. This configures the pi as a wifi-router.
  Make sure that ``uiot_ap_password`` is longer than 8 characters.

  If you have another USB-wifi stick, and want to use Internet via WiFi
  connect this wifi stick to the pi and configure ``uiot_wifi_name``,
  ``uiot_wifi_password``, and ``uiot_wifi_user`` correspondingly
  to your WiFi network.

  If you have access to ethernet (for example a free ethernet lan port on your
  router), connect the Pi to this ethernet - no extra configuration is necessary
  for this.

- Put the sd-card into a Raspberry Pi 3 and power it up (you can also put it into
  a Raspberry Pi Zero W, however there are some advanced configuration options
  You need to use to make that work fluently).

- In Windows, install `Moba xterm <https://mobaxterm.mobatek.net/>`__. On MacOS,
  make sure, you have `iTerm2 <https://iterm2.com/>`__ and
  `XQuartz <https://www.xquartz.org/>`__ installed. Linux
  will work out of the box.

- You should now see your ulnoiot wifi network as specified in ``uiot_ap_name``.
  Connect your computer (laptop or desktop pc) to this wifi network
  (use the password set in
  ``uiot_ap_password``). If everything was configured correctly you should still
  have internet on your computer.

- Connect to the ulnoiotgw via ssh. Make sure to enable X forwarding to have
  the clipboard working transparently.

  The command for Mac and Linux is:

  ``ssh -X pi@ulnoiotgw``

  The command on Windows will be (and can be graphically configured in MobaSSH):

  ``ssh -X pi@192.168.12.1``

  The default password for the user pi is ``ulnoiot``

  Consider changing it immediately entering the command ``passwd``

- Enter (and run = hit enter) the command ``ulnoiot upgrade`` to make sure that
  you have the latest version of ulnoiot.

- At one point, also make sure to run ``sudo raspi-config`` and chose to resize the
  hd in the advanced options.

- Check out the `short tmux help </doc/tmux-help.txt>`__,
  pressing the ctrl-key and a-key simultanously,
  releasing them and then pressing the h-key.

If you have trouble following this, make sure to checkout the tutorials on
youtube.

you can now continue with `First IoT Nodes`_


Installation on Linux
+++++++++++++++++++++

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
  (if packages are missing, fix dependencies and try to run
  ``ulnoiot install clean``)

- After successfully entering ulnoiot (the prompt should have changed colors and
  show ulnoiot in red, white, and black), start configuring your first IoT node,
  see `First IoT Nodes`_


First IoT Nodes
---------------

For this section, we assume that you have successfully set-up the ulnoiot
configuration management environment.

- Consider to configure  etc/ulnoiot.conf
  and run ``accesspoint`` and ``mqtt_broker``. If you installed from the
  Raspberry Pi image, this should not be necessary.

- Copy the folder ``lib/system_templates`` to a project directory,
  you can rename
  system_templates to a project name (i.e. iot-test-project)

- Rename the included node_template to a name for the node you want to
  configure (i.e. onboard_blinker)

- Adapt and configure system.conf and node.conf. Especialy make sure to add the
  correct board in node.conf. If you use a Wemos D1 Mini (this is the default),
  no change is necessary here.

- Now change into your node directory, connect an esp8266 based microcontroller
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
However, try first to add some devices manually at the console command prompt,
check and browse the help for available devices. Type ``run()`` to activate
these devices and then use the ``mqtt_all`` and ``mqtt_send`` tools to watch and
interact.
Try also ``help("autostart.py")`` at the console prompt.



External Resources
------------------

This project would not have been possible without a thriving open source
community around the Internet of Things. We make a lot of use of the following
tools:

- `The Tilde Texteditor <https://os.ghalkes.nl/tilde>`__
- `create_ap <https://github.com/oblique/create_ap>`__ forked for ulnoiot
  `here <https://github.com/ulno/create_ap>`__.
- `mosquitto <https://mosquitto.org/>`__.
- `mpfshell <https://github.com/wendlers/mpfshell>`__ forked for ulnoiot
  `here <https://github.com/ulno/mpfshell>`__.
- `micropython  <https://micropython.org/>`__
- `node-red <https://nodered.org>`__
- `rpi-clone <https://github.com/billw2/rpi-clone>`__

As ulnoiot relies heavily on MQTT, it also integrates very easily with
community home automation software like
`home-assistant <http://home-assistant.io>`__ and
`openhab <https://openhab.org>`__.


Support
-------

You can find the developers and people using ulnoiot on 
`#ulnoiot:matrix.org <https://riot.im/app/#/room/#ulnoiot:matrix.org>`__ 
or respectively riot in the group ulnoiot and might get answers there in 
real time.
Feel also free to file and discuss issues, problems, and potential new features
at http://github.com/ulno/ulnoiot.
Please feel also free to contact ulno directly via http://contact.ulno.net,
especially if you want to use ulnoiot in teaching yourself, or have somebody
teach the use and opportunities of ulnoiot to you or your organization in
person.


Further Documentation
---------------------

- `Some classes where ulnoiot is used <https://ulno.net/teaching/iot/>`__
- `micropython reference
  <https://docs.micropython.org/en/latest/esp8266/esp8266/quickref.html>`__.
- For a very light introduction in general python, take a look at
  `this <https://docs.python.org/3/tutorial/introduction.html>`__.
- For any problems or just being social, visit us on matrix/riot: `#ulnoiot:matreix.org <https://riot.im/app/#/room/#ulnoiot:matrix.org>`__
