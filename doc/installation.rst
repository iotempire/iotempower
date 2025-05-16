============
Installation
============

There are several ways to get the IoTempower configuration management software
up and running. The easiest is to run our install script like described below.



Linux, MacOS, Termux and WSL 2
------------------------------

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

Docker/Podman
-------------

To install IoTempower with docker or podman, enter a Linux environment
and type or paste the following (this is a bit trickier in Windows than
on other systems).

.. code-block:: bash

   curl -L docker.iotempower.us | bash -

Respectively for podman:

.. code-block:: bash

   curl -L podman.iotempower.us | bash -

Be aware that this environment does not yet have access to the serial ports
(and will never have in Windows),
therefore you will only be able to flash via the network via rfc2217.
     
Good news for our Windows users: The docker container is now also easy to install 
if you have Docker Desktop installed and WSL 2 Ubuntu integration activated - 
running the above command should be all you need after setting up docker
and ubuntu in wsl 2. Then you can start iot in your WSL 2 ubuntu environment.

The install script tries to install the docker starter script as iot. If this did not
succeed take a look at examples/scripts/iot-docker or iot-podman and take them as
an executable template to enter your iot container environment with the correctly mounted
folder. If you use these scripts, you can use them exactly like the iot script in a 
native installation.


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

Instead of a Raspberry Pi you can use a laptop as an accesspoint for IoT instead. You can give the 
gateway internet via phone(tethering)
or cable connected to the router.
We successfully used `Manjaro <https://manjaro.org/>`_ in the IoT class of 
spring 2024 at the University of Tartu. It worked well due to its ease of installation
and provided a fimiliar base for the students with the KDE plasma desktop.
It turned out still performant on the used low-powered laptops.

A guide for Manjaro gateway setup can be found `here <manjaro-gateways-setup.rst>`_


Installation, using existing router
-----------------------------------

Of course, you don't have to activate the gateway function of IoTempwoer and just use it
locally on your desktop or one of your servers.

In class, we are often using the GL.Inet Mango MT300 v2. We will add some
more information about this system architecture here soon.

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
