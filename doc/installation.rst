============
Installation
============

There are several ways to get the IoTempower configuration management software
up and running. The easiest is to run our install script like described below.



Linux, MacOS, Termux and WSL
----------------------------

For all systems, make sure you have **git** and **curl** installed.

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

After you have **git** and **curl**, Install IoTempower in the terminal with the following:
 
.. code-block:: bash

   curl -L https://now.iotempower.us | bash -

Hit enter on all questions for default settings, and if you want full features (recommended).

Convenience tools are optional but we recommend them because Midnight Commander is nicer file
managemer in the command line interface and integrates nicely with IoTempower.
Micro, and tilde are nicer/easier editors than nano or vim as they support Windows-like
keyboard shortcuts for select, copy, past, save, and quit.

*Alternatively, you can install everything directly without the prompt questions by running:*

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -s -- --default

After the installation is finished, open a new terminal, and just run ``iot`` from anywhere
(you can also run ``bash run`` in the iot directory if things go wrong,
but only for debugging purposes).

After successfully entering IoTempower (the prompt
should have changed and started now with IoT),
start configuring your first IoT node,
see `First IoT Node <first-node.rst>`_.

Manual Installation
-------------------

If you do not trust the install via curl, execute the following (and verify after
cloning the content of bin/iot_install) to do a manual install:

.. code-block:: bash

   cd # go to home directory
   my_iot_folder=iot
   git clone https://github.com/iotempire/iotempower "$my_iot_folder"
   cd "$my_iot_folder"
   bash run


Installation on Raspberry Pi with Image
---------------------------------------

You can download and flash a pre-prepared Raspberry Pi image to an sd card
and run the IoTempower (gateway and configuration management software) on a Raspberry Pi.
Please follow the instructions in the following link:
`Installation on Raspberry Pi <installation-raspberry-pi.rst>`_
   
Manjaro gateway setup
---------------------------------------

Instead of a Raspberry Pi you can use a laptop as an accesspoint for IoT instead. You can give the gateway internet via phone(tethering)
or cable connected to the router.
`Manjaro <https://manjaro.org/>`_ was successfully used using this practice in the IoT class of spring 2024 at the University of Tartu and chosen due to its ease of installation and a good base for the KDE plasma desktop that was giving familiarity to the students and still performant on low powered laptops.

Guide for Manjaro gateway setup can be found `here <manjaro-gateways-setup.rst>`_

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
