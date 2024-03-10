Please also check out the tutorial videos for this setup on ulno's youtube
channel(s): https://www.youtube.com/@ut-teaching-ulno/search?query=gateway


Installation on Linux (and WSL)
+++++++++++++++++++++++++++++++

The steps for WSL (Windows Subsystem for Linux) and Linux should be the same.
For information on how to run IoTempower on a Raspberry Pi, 
please go to `this link </doc/installation.rst>`__.

We highly recommend against using WSL 2 as the networking is currently
a total mess and serial ports are nearly unsupported.
WSL 1 kind of works, though a powerful computer (4 cores and min. 16GB) should still yield better
results with a lightweight Linux (like xubuntu or xfce manjaro) in a
virtual machine (using for example virtual box with bridged networking).
If you don't have a computer running a dedicated Linux, consider dual
booting Linux.

1. On an Arch based system (like vanilla Arch or Manjaro), assuming you have yay installed,
   run the following commands:
 
   .. code-block:: bash

      # On your terminal
      cd  # go into your home directory also referred to as ~ or $HOME (it is something like /home/user)
      sudo pacman -Syyu  # make sure system is up to date
      sudo pacman -S git 
      curl -L https://bit.ly/iotempower | bash -
      # Hit enter on all questions for default settings and if you want full features (recommended)
      # convenience tools are optional but we recommend them because 
      # mc is nicer file management in cli, and micro and tilde are nicer editor than nano or vim
      # if you run on a native Arch/Manjaro (not in WSL) make sure you stop and disable mosquitto
      # (you can skip this if you like the default password-less mosquitto setup, but be warned)
      sudo systemctl stop mosquitto
      sudo systemctl disable mosquitto

   
   On a Debian based Linux like Ubuntu or Mint (WSL or native), 
   run the following commands:
 
   .. code-block:: bash
   
      # On your terminal
      cd  # go into your home directory also referred to as ~ or $HOME (it is something like /home/user)
      sudo apt-get update  # make sure system is up to date
      sudo apt install git # if not installed yet
      # Install IoTempower by running the following line:
      curl -L https://bit.ly/iotempower | bash -
      # Hit enter on all questions for default settings and if you want full features (recommended)
      # convenience tools are optional but we recommend them because 
      # mc is nicer file management in cli, and micro and tilde are nicer editor than nano or vim
      # if you run on a native Ubuntu (not in WSL) make sure you stop and disable mosquitto
      # (you can skip this if you like the default password-less mosquitto setup, but be warned)
      sudo systemctl stop mosquitto
      sudo systemctl disable mosquitto

   


2. Add port permissions to avoid permission issues (replace ``<your-username>`` with the username you chose/have).
   
   - In Debian based (Ubuntu, Mint): ``sudo usermod -a -G dialout <your-username>``
   
   - In Arch based (Arch, Manjaro): ``sudo usermod -a -G uucp <your-username>``
   
   Restart or re-login into Linux or restart terminal (WSL 1).

   
3. After the installation, you can just run ``iot`` from anywhere (if the binary was created)
   or you can also run ``bash run`` in the iot directory .

   (If packages are missing, fix dependencies and try to run
   ``iot install clean``)


4. After successfully entering IoTempower (the prompt
   should have changed and started now with IoT),
   start configuring your first IoT node,
   see `First IoT Node <first-node.rst>`_.


Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
