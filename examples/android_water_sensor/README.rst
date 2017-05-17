This example can be used to build a completely 
mobile demo, in which your Android phone serves as your wireless.
It can run mosquitto in termux and even react with mqtt_action
and teh local do_alarm on different events published.

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

- Configure a wemos water sensor (refer to provided example 
  directory water-sensor01)

- Run run from this directory
