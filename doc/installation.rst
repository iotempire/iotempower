============
Installation
============

There are several ways to get the IoTempower configuration management software
up and running:

Please also check out the tutorial videos for this setup on ulno's youtube
channel(s): https://www.youtube.com/@ut-teaching-ulno/search?query=gateway


Linux, MacOS, Termux and WSL
----------------------------

For all systems, make sure you have **git** and **curl** install.

* Arch-based system (like vanilla Arch or Manjaro)

 ``sudo pacman -S git curl`` 

* Debian-based system (like Ubuntu or Mint). WSL is probably similar to this.
  We highly recommend **against** using WSL 2 as the networking is currently
  a total mess and serial ports are nearly unsupported.

 ``sudo apt-get install git curl``

* Termux-based system (https://termux.dev) can be used on Android
  
 ``apt-get install git curl``
 
* MacOS install homebrew (https://brew.sh/) and then run 
   
 ``brew install git curl``

After you have **git** and **curl**, Install IoTempower with the following:
 
.. code-block:: bash

   # On your terminal, run the following line:
   curl -L https://bit.ly/iotempower | bash -

Hit enter on all questions for default settings and if you want full features (recommended).

Convenience tools are optional but we recommend them because. Midnight Commander is nicer file management in cli. Micro, and tilde are nicer editor than nano or vim.

*Alternatively, you can install everything directly without the prompt questions by running:*

.. code-block:: bash

   # On your terminal, run the following line:
 ``curl -L https://bit.ly/iotempower | bash -s -- --quiet``

After the installation is finished, open a new terminal, and just run ``iot`` from anywhere
or you can also run ``bash run`` in the iot directory.


After successfully entering IoTempower (the prompt
should have changed and started now with IoT),
start configuring your first IoT node,
see `First IoT Node <first-node.rst>`_.


Installation on Raspberry Pi with image
---------------------------------------

You can download and flash a pre-prepared Raspberry Pi image to an sd card
and run the IoTEmpower (gateway and configuration management software) on a Raspberry Pi. Please follow the instructions in the following link:
`Installation on Raspberry Pi <installation-raspberry-pi.rst>`_
   

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
