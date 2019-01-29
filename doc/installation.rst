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

- If you have another USB-wifi stick, and want to use Internet via WiFi
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

You can now continue with `First IoT Nodes <first-node.rst>`_.


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
  see `First IoT Node <first-node.rst>`_.

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.