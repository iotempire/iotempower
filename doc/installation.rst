Installation
------------

There are two ways to get the IoTempower configuration management software
up and running:

1. Downloading and flashing a pre-prepared Raspberry Pi image to an sd card
   and running the gateway and configuration management software from there.

2. Setting up IoTempower in your own Linux environment:
   `Installation on Linux (and WSL)`_

Please also check out the tutorial videos for this setup on ulno's youtube
channel: https://www.youtube.com/results?search_query=ulno.net+iotempower+installation


Installation on Raspberry Pi from Pre-Prepared Image
++++++++++++++++++++++++++++++++++++++++++++++++++++

This section is currently outdated, proceed with caution (look better at Linux installation).

Tutorial videos for setup:

- `On MacOS <https://www.youtube.com/watch?v=oHM-ojoST-c>`__

- `On PC/Windows <https://youtu.be/DZ-PwxIc8wY>`__

- On Linux: Please follow the installation steps below (or file an issue to ask
  for specific Linux video).



Installation step by step:

- `Install the IoTempower image onto an SD-card <image-pi.rst>`_

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

- https://iotgateway (or https://iotgateway.local or
  sometimes https://192.168.12.1). Accept
  the security exception for the locally generated security certificate. You
  should now see a link to the filesystem through cloud commander (cloudcmd)
  and an IoT testsystem on the pi,
  as well as links to the Node-RED installation.

- On the raspberry pi IoTempower installation, you can use cloudcmd to access
  and interact with your system.

- If you are asked for a user, use ``iot``, if you are asked for a password
  use ``iotempire``.

- ssh access (this is for advanced users, usually you can just use the
  browser):

  - For ssh access in Windows,
    install `Moba xterm <https://mobaxterm.mobatek.net/>`__ 
    or `Git and integrated ssh for Windows <https://git-scm.com/download/win>`__.

  - On MacOS,
    make sure, you have `iTerm2 <https://iterm2.com/>`__ and optionally
    `XQuartz <https://www.xquartz.org/>`__ installed.

  - Linux will work out of the box.

  - On Android use termux.

  - For Mac or Linux you can use the built in ssh commands in the terminal.
    Make sure to enable X forwarding to have
    the clipboard working transparently.

    The command for Mac and Linux is:

    ``ssh -X iot@iotgateway``

    The command on Windows will be (and can be graphically
    configured in MobaSSH):

    ``ssh -X iot@192.168.12.1``

    The default password for the user IoTempower is ``iotempire``

    Consider changing it immediately entering the command ``passwd``

  - At one point, also make sure to run ``sudo raspi-config`` and
    chose to resize the
    hd in the advanced options.

..  - Check out the `short tmux help </doc/tmux-help.txt>`__,
    pressing the ctrl-key and a-key simultanously,
    releasing them and then pressing the h-key.

  - Run in ssh or the terminal (type and hit enter) the command
    ``iot upgrade`` to make sure that
    you have the latest version of iotempower.


You can now continue with `First IoT Nodes <first-node.rst>`_.


Installation on Linux (and WSL)
+++++++++++++++++++++++++++++++

The steps for WSL (Windows Subsystem for Linux) and Linux should be the same. For information on how to run IoT Empower on a Raspberry Pi, 
please go to `this link </doc/installation.rst>`__.

We highly recommend against using WSL 2 as the networking is currently
a total mess and serial ports are nearly unsupported.
WSL 1 kind of works, though a powerful computer (4 cores and min. 16GB) should still yield better
results in with a lightweight Linux (like xubuntu or xfce manjaro) in
virtual machine (using for example virtual box with bridged networking).
If you don't have a computer running a dedicated Linux, consider dual
booting Linux.

1. On a Debian based Linux like Ubuntu or Mint (WSL or native), 
   run the following commands:

   .. code-block:: bash
   
      cd  # go into your home directory also referred to as ~ or $HOME
      sudo apt-get update  # make sure system is up to date
      sudo apt install git mc mosquitto mosquitto-clients virtualenv iptables 
      sudo apt install bridge-utils hostapd dnsmasq nodejs build-essential npm
      sudo npm install -g terminal-kit  # this is ugly as it uses root, but the simplest way
      # if you run on a native Ubuntu (not in WSL) consider running
      # (you can skip this if you like the default password-less mosquitto setup, but be warned)
      sudo systemctl stop mosquitto
      sudo systemctl disable mosquitto

   On an Arch based system (like vanilla Arch or Manjaro), assuming you have yay installed,
   run the following commands:

   .. code-block:: bash

      cd  # go into your home directory also referred to as ~ or $HOME
      sudo pacman -Syyu  # make sure system is up to date
      sudo pacman -S git mc mosquitto python-virtualenv 
      sudo pacman -S iptables bridge-utils hostapd dnsmasq nodejs npm
      # terminal-kit installation is done locally in iot environment and works on arch
      # but if you want it globally, you can consider sudo npm install -g terminal-kit
      # if you run on a native Arch/Manjaro (not in WSL) consider running
      # (you can skip this if you like the default password-less mosquitto setup, but be warned)
      sudo systemctl stop mosquitto
      sudo systemctl disable mosquitto


2. Add port permissions for avoiding permission issues (replace ``<your-username>`` with the username you chose/have).
   
   - In Debian based (Ubuntu, Mint): ``sudo usermod -a -G dialout <your-username>``
   
   - In Arch based (Arch, Manjaro): ``sudo usermod -a -G uucp <your-username>``
   
   Restart or re-login into Linux or restart terminal (WSL 1).

   
3. Clone the IoTempower repository using git into the iot folder in home directory

   .. code-block:: bash

      cd  # go into your home directory
      git clone https://github.com/iotempire/iotempower iot

      # if you are an IoTempower developer, use the following instead
      git clone git@github.com:iotempire/iotempower iot

4. Copy ``iot/examples/scripts/iot`` into your ``bin`` folder
   (either ``~/bin`` or ``~/.local/bin``) and
   adapt the path in it to reflect the location where you cloned the IoTempower
   (if you followed the advice above: `~/iot` which is the default in the sample script
   or if you just cloned it without specifying the folder iot,
   ``export IOTEMPOWER_ROOT="$HOME/iotempower"`` if your IoTempower
   directory is directly ``iotempower`` in your home directory)

   If you use tmux or byobu with bash consider sourcing ``lib/shell_starter/iotempower.bash``
   in your .bashrc. (If you did not understand the last sentence, ignore it.)

5. Change into your iotempower directory (``cd ~/iot``) and run ``bash run``,
   you should get a welcome message. Accept the installation of any extra packages.
   After the installation, you can just run ``iot`` from anywhere (if you created the binary as described in 3)
   or you can also run ``bash run`` in the IoTempower directory again.

   (If packages are missing, fix dependencies and try to run
   ``iot install clean``)


6. After successfully entering IoTempower (the prompt
   should have changed and start now with IoT),
   start configuring your first IoT node,
   see `First IoT Node <first-node.rst>`_.


Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
