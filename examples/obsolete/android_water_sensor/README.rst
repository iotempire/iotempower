This example can be used to build a completely 
mobile demo, in which your Android phone serves as your wireless accesspoint and
IoT gateway. Android can run the mqtt server mosquitto in termux and even react
with mqtt_action and the local do_alarm on different events published.

The alarm file is by Jomelle Jager (http://jomelle.wordpress.com)
downloaded from freesound.org.

Installation
============

- Install ulnoiot in termux

    - Install git in termux (``apt install git``)
    - Clone from git (``git clone https://github.com/ulno/ulnoiot``)
    - Run examples/scripts/termux-ulnoiot.sh to install termux dependencies
    - Run ``bash run`` in cloned directory
    - Install examples/scripts/ulnoiot as a script in a locally
      accessible binary path and modify ULNOIOT_ROOT in it to point to
      the cloned directory

- Copy and adjust etc/ulnoiot.conf

- Configure a wemos d1 mini with attached analog water sensor:

  - login (for example via ssh from termux or just from your desktop)
    to a PC or single board computer (SBC - like the raspberry pi) with ulnoiot
    install and attach a the wemos d1 mini to it

  - cd to provided example directory android_water_sensor

    ``cd examples/android_water_sensor``

  - adjust ulnoiot.conf and especially set the wifi name and essid for your
    phone tethered wifi network

  - start a localized ulnoiot environment for that specific example

    ``ulnoiot_local``

    This sources the local ulnoiot.conf (the one in the current directory
    to allow a specialized environment for this example - the water sensor)
    and adjusts the global configuration for it.

  - cd to the node directory water-sensor01

    ``cd water-sensor01``

  - type ``initialize`` and flash the attached wemos d1 mini

  - you can later update

- Enable wifi tethering on your phone

- In termux change to this water-sensor directory and, run ``run`` from this
  directory to start all the iot gateway environment on your phone

TODO: link images and youtube video