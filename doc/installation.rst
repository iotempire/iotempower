============
Installation
============

There are several ways to get the IoTempower configuration management software
up and running. **A native (classic) Linux installation is always preferred**
and offers the best experience, maintainability, and full device access.

.. note::
   **Transition Notice:** For Windows users, we now offer WSL2 with USB device
   support via ``usbipd-win`` (installed using ``winget``) as a new recommended 
   option. This provides a much better experience than Docker on Windows.
   Docker and Raspberry Pi installations are being deprecated—see notes below.


Linux, MacOS, and Termux (Preferred)
------------------------------------

**A native Linux installation is the most robust and recommended approach.**

For all systems, make sure you have **git** and **curl** installed.

* Arch-based system (like vanilla Arch or Manjaro)

  ``sudo pacman -S git curl``

* Debian-based system (like Ubuntu or Mint)

  ``sudo apt-get install git curl``

* Termux-based system (https://termux.dev) can be used on Android

  ``apt-get install git curl``

* MacOS install homebrew (https://brew.sh/) and then run

  ``brew install git curl``

After you have **git** and **curl**, install IoTempower in the terminal with the following:

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -

Hit enter on all questions to use default settings and enable full features (recommended).

Convenience tools are optional but we recommend them because Midnight Commander is nicer file
manager in the command line interface and integrates nicely with IoTempower.
Micro, and tilde are nicer/easier editors than nano or vim as they support Windows-like
keyboard shortcuts for select, copy, paste, save, and quit.

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


Windows Host with WSL2 + usbipd-win
-----------------------------------

.. note::
   **New Option:** For Windows 10 (2004+) and Windows 11 users, WSL2 combined
   with ``usbipd-win`` provides a viable alternative to a full Linux installation.
   This setup allows USB serial device access (for ESP32, ESP8266, etc.) from 
   within WSL2, enabling full flashing and device communication.

**Step 1: Install WSL2 and Ubuntu (or Debian)**

In a Windows PowerShell (run as Administrator):

.. code-block:: powershell

   wsl --install

Restart as instructed. Then open Ubuntu from the Start menu and run:

.. code-block:: bash

   sudo apt-get update
   sudo apt-get install git curl
   curl -L https://now.iotempower.us | bash -

**Step 2: Enable USB serial device access**

To access USB devices (like ESP32/ESP8266) from WSL2, install ``usbipd-win``
on the Windows host using ``winget``. ``winget`` is pre-installed on Windows 11
and Windows 10 version 2004 and later. If unavailable, update "App Installer"
from the Microsoft Store.

In a Windows command prompt or PowerShell (**not** WSL):

.. code-block:: powershell

   winget install --interactive --exact dorssel.usbipd-win

After installation, plug in your device and run:

.. code-block:: powershell

   usbipd list
   usbipd attach --busid <BUSID_FROM_LIST>

Your device will now appear under ``/dev/ttyUSB0`` or similar in WSL2.
Use IoTempower as normal from your WSL2 Linux shell.


Docker/Podman (Deprecated)
--------------------------

.. warning::
   **Docker Desktop is deprecated as a recommended installation method.**

   Docker Desktop's license terms changed significantly in 2024, creating
   uncertainty about eligibility for educational and academic use. Many
   organizations and individuals may no longer be eligible for free use.
   For more information on the licensing controversy, see discussions in
   the community.

   Additionally, Docker on Windows via WSL2 often introduces technical issues,
   and hardware (serial/USB) support is severely limited. Podman shares 
   similar limitations.

   **We recommend using a native Linux installation or WSL2 with usbipd-win
   instead of Docker.**

If you *must* use containers, in a Linux environment:

.. code-block:: bash

   curl -L docker.iotempower.us | bash -

Respectively for podman:

.. code-block:: bash

   curl -L podman.iotempower.us | bash -

Be aware that this environment does not have access to the serial ports
(and will never have in Windows),
therefore you will only be able to flash via the network via rfc2217.

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


Installation on Raspberry Pi (Deprecated)
-----------------------------------------

.. warning::
   **Raspberry Pi images and installation are deprecated.**

   The preferred and best-supported method for IoTempower is a maintained,
   standard Linux PC or VM. Raspberry Pi images will no longer be actively
   maintained and are not recommended for new deployments.

   For gateway and infrastructure setups, consider using Manjaro (see below)
   or another mainstream Linux distribution on standard hardware.

If you still wish to use a Raspberry Pi, you can download and flash a 
pre-prepared Raspberry Pi image to an SD card and run IoTempower on it.
Please follow the instructions in the following link:
`Installation on Raspberry Pi <installation-raspberry-pi.rst>`_


Manjaro gateway setup
---------------------

Instead of a Raspberry Pi, you can use a laptop as an access point for IoT. You can provide the
gateway internet access via phone (tethering)
or cable connected to the router.
We successfully used `Manjaro <https://manjaro.org/>`_ in the IoT class of
spring 2024 at the University of Tartu. It worked well due to its ease of installation
and provided a familiar base for the students with the KDE plasma desktop.
It turned out still performant on the used low-powered laptops.

A guide for Manjaro gateway setup can be found `here <manjaro-gateways-setup.rst>`_


Installation, using existing router
-----------------------------------

Of course, you don't have to activate the gateway function of IoTempower and just use it
locally on your desktop or one of your servers.

In class, we are often using the GL.Inet Mango MT300 v2. We will add some
more information about this system architecture here soon.

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
