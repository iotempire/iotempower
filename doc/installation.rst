Installation
------------

There are two ways to get the ulnoiot configuration management software
up and running:

1. Downloading and flashing a pre-prepared Raspberry Pi image to an sd card
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

- `Install the UlnoIoT image onto an SD-card <image-pi.rst>`_

- `Set up the credentials of the WiFi-Router on the Pi
  <quickstart-pi.rst#installation#setting-up-the-wifi-router-on-the-pi>`_

- If you have another USB-wifi stick, and want to use Internet via WiFi
  connect this wifi stick to the pi and make corresponding changes to the
  file ``/boot/wifi-in.txt``, setting ``WiFi Name/SSID``, ``WiFi Password``,
  and eventually the ``WiFi business network user``.

  Careful, lots of WiFi sticks do not work properly on the
  Raspberry Pi. Make sure you have verified that it works under the normal
  Raspberry Pi desktop environment.

  Much preferred is connecting the Pi to ethernet. It avoids all the
  aforementioned potential WiFi problems. If you have access to ethernet
  (for example a free ethernet lan port on your
  router), connect the Pi to this ethernet - no extra configuration is
  necessary for this.

- You can now connect to the pi gateway via a web browser or ssh

- https://ulnoiotgw (or https://ulnoiotgw.local or
  sometimes https://192.168.12.1). Accept
  the security exception for the locally generated security certificate. You
  should now see a link to the filesystem through cloud commander
  and an IoT testsystem on the pi,
  as well as links to the Node-RED installation.

- On the raspberry pi ulnoiot installation, you can use cloudcmd to access
  and interact with your system.

- If you are asked for a user, use ``ulnoiot``, if you are asked for a password
  use ``iotempire``.

- ssh access (this is for advanced users, usually you can just use the
  browser):

  - For ssh access in Windows,
    install `Moba xterm <https://mobaxterm.mobatek.net/>`__.

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

    The command on Windows will be (and can be graphically
    configured in MobaSSH):

    ``ssh -X ulnoiot@192.168.12.1``

    The default password for the user ulnoiot is ``iotempire``

    Consider changing it immediately entering the command ``passwd``

  - At one point, also make sure to run ``sudo raspi-config`` and
    chose to resize the
    hd in the advanced options.

..  - Check out the `short tmux help </doc/tmux-help.txt>`__,
    pressing the ctrl-key and a-key simultanously,
    releasing them and then pressing the h-key.

  - Run in ssh or the terminal (type and hit enter) the command
    ``ulnoiot upgrade`` to make sure that
    you have the latest version of ulnoiot.


You can now continue with `First IoT Nodes <first-node.rst>`_.


Installation on Linux
+++++++++++++++++++++

- install dependencies:
  ``sudo apt install git mc mosquitto mosquitto-clients virtualenv
  iptables bridge-utils hostapd dnsmasq nodejs``

- disable the mosquitto server (you can skip this if you like the default
  password-less mosquitto setup, but be warned):
  ``sudo systemctl stop mosquitto; sudo systemctl disable mosquitto``

- setup ulnoiot: clone this repository

  - If you just want read-only access type in a folder of your choice:
    ``git clone https://github.com/ulno/ulnoiot``

  - If you are a ulnoiot developer, use
    ``git clone git@github.com:ulno/ulnoiot``

- make ulnoiot runnable -> copy examples/scripts/ulnoiot into your bin folder
  and adapt
  the path in it to reflect the location where you cloned ulnoiot. If you use
  tmux or byobu with bash consider sourcing ``lib/shell_starter/ulnoiot.bash``
  in your .bashrc.

- start ulnoiot and agree and wait for dependencies to be downloaded
  (if packages are missing, fix dependencies and try to run
  ``ulnoiot install clean``)

- After successfully entering ulnoiot (the prompt
  should have changed colors and
  show ulnoiot in red, white, and black),
  start configuring your first IoT node,
  see `First IoT Node <first-node.rst>`_.

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.