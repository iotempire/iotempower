=======
ulnoiot
=======

Attention: this is ulnoiot-tng (The Next Generation, not anymore based on 
micropython, now using `PlatformIO <platform.io>`__ as a base to generate
firmware and code).


Introduction
------------

*ulnoiot* (pronounced: "You'll know IoT") is a framework and environment
for making it easy for everyone to explore and develop for the
Internet of Things (IoT)
-- easy for tinkerers, makers, programmers, hobbyists, students, artists,
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
- Laptops running Ubuntu Linux 17.04 and 18.04

We are trying to provide virtualbox images as soon as we find time and/or volunteers.

We are also working on verifying that ulniot works well on Orange-Pi Zero to
allow more cost-effective solutions to use ulnoiot.

Currently the following esp8266-based devices are supported:

- Wemos D1 Mini
- NodeMCU
- Espresso Lite V2
- Sonoff and Sonoff Touch
- There is an esp8266 generic option for other esp8266-based boards.
- We expect to support esp32 boards very soon - let us know if you want to help
  making this possible.

The part of ulnoiot running on the esp8266 is a standalone C++-based firmware
managed by `PlatformIO <http://platform.io>`__. However, ulnoiot abstracts a
lot of the burden of repetitive device management away from the user so that
attaching a device to a node usually boils down to just writing one line of
code, which you can adapt from plenty of examples.
Earlier versions were based on `micropython <http://www.micropython.org/>`__,
however, porting some of the C++-based Arduino device driver libraries, managing 
remote access, updates, dealing with very little memory, and a slightly defunct
community, made mangement very hard leading us to the decision to switch to an
admittedly harder to program environment, however, we earned the access to the huge
and active Arduino community making problem solving and extensions much easier. We
do not regret the switch.

There was some initial effort in creating a starter development kit for
the Wemos D1 Mini - you can see more information `here
</doc/shields/wemosd1mini/devkit1/README.rst>`__. However, we are now more
focusing on using cheap hardware from various 37 in 1 sensor kits, which can
still easily be plugged together.



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

- install: (re-)install the ulnoiot environment (if you specify clean,
  it re-installs)

- ``console_serial``: open a serial console to see debug output of a
  serially (locally) connected node

- ``initialize``: initialize a current node which is in reconfiguration mode or
  flashes a serially connected node and sets all intial configuration parameters
  like wifi credentials and security keys


- ``deploy``: updates software of a ulnoiot node after changes. The update
    is done over the network (OTA) 


Installation
------------

There are two ways to get the ulnoiot configuration management software
up and running:

1. Downloading and flashing a pre-prepared raspberry pi image to an sd card
   and running the gateway and configuration management software from there.

2. Setting up ulnoiot in your own Linux environment:
   `Installation on Linux`_

Please also check out the tutorial videos for this setup on ulno's youtube
channel: https://www.youtube.com/results?search_query=ulno.net+ulnoiot+installation


Installation on Raspberry Pi from Pre-Prepared Image
++++++++++++++++++++++++++++++++++++++++++++++++++++

Tutorial videos for setup:

- `On MacOS <https://www.youtube.com/watch?v=oHM-ojoST-c>`__

- `On PC/Windows <https://youtu.be/DZ-PwxIc8wY>`__

- On Linux: Please follow the installation steps below (or file an issue to ask
  for specific Linux video).

  

Installation step by step:

- Download the Raspberry Pi image from here: https://goo.gl/bVgLMr
  (you might later want to use ssh and upgrade node-red dashboard
  on this image, turns out it is buggy in using the charts. Do:
  ``sudo npm install -g --unsafe-perm node-red-dashboard``)

- Make sure the sha256-checksum of the image is correct. It should be:

  50e55874c94ba943005cfa85a4afd3cc40c94257e05e4a7f28ca70ffc60dc009

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
  to your WiFi network. Careful, lots of WiFi sticks do not work properly on the
  raspberry pi. Make sure you have verified that it works under the normal
  raspberry desktop environment.
  
  Much preferred is connecting the pi to ethernet. It avoids all the
  aforementioned potential WiFi problems. If you have access to ethernet 
  (for example a free ethernet lan port on your
  router), connect the Pi to this ethernet - no extra configuration is necessary
  for this.

- Put the sd-card into a Raspberry Pi 3 and power it up (you can also put it into
  a Raspberry Pi Zero W, however there are some advanced configuration options
  You need to use to make that work fluently).

- You should now see your ulnoiot wifi network as specified in ``uiot_ap_name``.
  Connect your computer (laptop or desktop pc) to this wifi network
  (use the password set in
  ``uiot_ap_password``). If everything was configured correctly you should still
  have internet on your computer.

- You can now connect to the pi gateway via a web browser or ssh
  
  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local). Accept
    the security exception for the locally generated security certificate. You
    should now see a link to the filesystem through cloud commander
    and an IoT testsystem on the pi,
    as well as links to the Node-RED installation.
    
    Cloudcmd allows you to open a small console through typing '
    
  - If you are asked for a user, use ``ulnoiot``, if you are asked for a password
    use ``iotempire``.

  - For ssh access in Windows, install `Moba xterm <https://mobaxterm.mobatek.net/>`__. 
  
  - On MacOS,
    make sure, you have `iTerm2 <https://iterm2.com/>`__ and
    `XQuartz <https://www.xquartz.org/>`__ installed.
    
  - Linux will work out of the box.
  
  - On Android use termux.

  - For Mac or Linux you can use the built in ssh commands in the terminal.
    Make sure to enable X forwarding to have
    the clipboard working transparently.

    The command for Mac and Linux is:

    ``ssh -X ulnoiot@ulnoiotgw``

    The command on Windows will be (and can be graphically configured in MobaSSH):

    ``ssh -X ulnoiot@192.168.12.1``

    The default password for the user ulnoiot is ``iotempire``

    Consider changing it immediately entering the command ``passwd``

  - At one point, also make sure to run ``sudo raspi-config`` and chose to resize the
    hd in the advanced options.

  - Check out the `short tmux help </doc/tmux-help.txt>`__,
    pressing the ctrl-key and a-key simultanously,
    releasing them and then pressing the h-key.

- Run in ssh or the console (type and hit enter) the command ``ulnoiot upgrade`` to make sure that
  you have the latest version of ulnoiot.


If you have trouble following this, make sure to checkout the tutorials on
youtube.

You can now continue with `First IoT Nodes`_.


Installation on Linux
+++++++++++++++++++++

- install dependencies:
  ``sudo apt install git mc mosquitto mosquitto-clients virtualenv iptables bridge-utils``

- disable the mosquitto server (you can skip this if you like the default
  password-less mosquitto setup, but be warned):
  ``sudo systemctl stop mosquitto; sudo systemctl disable mosquitto``

- setup ulnoiot: clone this repository

  - If you just want read-only access type in a folder of your choice:
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
  see `First IoT Nodes`_.


First IoT Nodes
---------------

For this section, we assume that you have successfully set-up the ulnoiot
configuration management environment.

- Consider to configure  etc/ulnoiot.conf
  and run ``accesspoint`` and ``mqtt_broker``. If you installed from the
  Raspberry Pi image, this should not be necessary as they are started
  automatically.

- Copy the folder ``lib/system_templates`` to a project directory,
  you can rename
  system_templates to a project name (i.e. iot-test-project)

- Rename the included node_template to a name for the node you want to
  configure (i.e. onboard_blinker)

- Adapt and configure system.conf and node.conf. Especialy make sure to add the
  correct board in node.conf. If you use a Wemos D1 Mini (this is the default),
  no change is necessary here, lots of users have the NodeMCU development board.
  If you want to use a NodeMCU, change the config to NodeMCU.

- Now change into your node directory, connect an (only one) esp8266 based microcontroller
  to your pc or raspberry/orange pi and type ``initialize serial``. This flashes and
  pre-configures the device. If you use the inbuilt wifi configuration (like
  described in uhelp wifi), just use ``initialize``
  to adopt the node via the network.

- If you like, access the debug console with ``console_serial`` (if only one esp is connected
  the serial port will be discovered automatically else supply it as usb1 or 
  acm2).

If something gets stuck, try to power cycle the esp8266.

``initialize`` sets up your wifi based on the settings in system.conf and also
encrypts the network connection.

At an ulnoiot-command prompt try typing ``uhelp`` and check the small manual.

Take a look at the setup.cpp in your node-folder and enable the onboardled.

Try also ``uhelp setup.cpp`` at the ulnoiot prompt.



External Resources
------------------

This project would not have been possible without a thriving open source
community around the Internet of Things. We make a lot of use of the following
tools:

- `PlatformIO <http://platform.io>`__
- `node-red <https://nodered.org>`__
- `mosquitto <https://mosquitto.org/>`__.
- `The Tilde Texteditor <https://os.ghalkes.nl/tilde>`__
- `create_ap <https://github.com/oblique/create_ap>`__ forked for ulnoiot
  `here <https://github.com/ulno/create_ap>`__.
- `FastLed rgb-strip library <https://github.com/FastLED/FastLED>`__
- `rpi-clone <https://github.com/billw2/rpi-clone>`__
- `cloudcmd <https://github.com/coderaiser/cloudcmd>`__

Old versions were also using these:
- `mpfshell <https://github.com/wendlers/mpfshell>`__ forked for ulnoiot
  `here <https://github.com/ulno/mpfshell>`__.
- `micropython  <https://micropython.org/>`__


As ulnoiot relies heavily on MQTT, it also integrates very easily with
other community home automation software like
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
- For any problems or just being social, visit us on matrix/riot: `#ulnoiot:matrix.org <https://riot.im/app/#/room/#ulnoiot:matrix.org>`__

Obsolete:
- `micropython reference
  <https://docs.micropython.org/en/latest/esp8266/esp8266/quickref.html>`__.
- For a very light introduction in general python, take a look at
  `this <https://docs.python.org/3/tutorial/introduction.html>`__.
  
