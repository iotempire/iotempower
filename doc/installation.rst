Installation
============

There are several ways to get the IoTempower configuration management software up and running. **A native (classic) Linux installation is preferred and offers the best experience, maintainability, and full device access.** For Windows, we now recommend WSL2 with USB device support via `usbipd-win` (installed with winget). **Docker and Raspberry Pi installations are deprecated** due to technical and licensing issues.

Linux, MacOS, Termux and WSL2 (Preferred)
------------------------------------------

For all systems, make sure you have **git** and **curl** installed.

* Arch-based system (like vanilla Arch or Manjaro)

  ``sudo pacman -S git curl``

* Debian-based system (like Ubuntu or Mint) – WSL2 with Ubuntu/Debian also follows this flow:
  ``sudo apt-get install git curl``

* Termux-based system (https://termux.dev) can be used on Android
  ``apt-get install git curl``

* macOS: install homebrew (https://brew.sh/) and then run
  ``brew install git curl``

After you have **git** and **curl**, install IoTempower in the terminal with:

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -

Hit enter on all questions for default settings and full features (recommended).

Convenience tools (like Midnight Commander, micro, tilde) provide easier file and text editing on the command line.

*Alternatively, install directly with default prompts:*

.. code-block:: bash

   curl -L https://now.iotempower.us | bash -s -- --default

After installation, open a new terminal and run ``iot`` from anywhere. (Or, for debugging, run ``bash run`` in the iot directory.)

After entering IoTempower for the first time (prompt should now indicate IoT), start configuring your first IoT node (see `First IoT Node <first-node.rst>`_).

Windows Host via WSL2 + usbipd-win (Recommended)
------------------------------------------------

*For Windows 11 and recent Windows 10, this is now the most robust and least-friction method.*

1. Install WSL2 and Ubuntu (or Debian):
   
.. code-block:: powershell
   
   wsl --install

   # Restart as instructed, then in Ubuntu:
   sudo apt-get update
   sudo apt-get install git curl
   curl -L https://now.iotempower.us | bash -

2. Enable USB serial device access (ESP32/ESP8266 etc):
   - Ensure you have Windows "App Installer" (winget) available (pre-installed on Windows 11, Win10 2004+).
   - In a Windows command prompt or PowerShell (not WSL):

.. code-block:: powershell

   winget install --interactive --exact dorssel.usbipd-win

   # Plug in your device, then:
   usbipd list
   usbipd attach --busid <BUSID_FROM_LIST>

   # Your device will now appear under /dev/ttyUSB0 or similar in WSL2

   - Use as normal from WSL2 Linux shell. Full flashing and device access is now supported.
   - If winget is not available, update "App Installer" from Microsoft Store.

Docker/Podman (Deprecated)
--------------------------

.. warning::
   **Docker Desktop is deprecated as a recommended install.**

   Docker Desktop's license terms for education and academia changed significantly in 2024, leading to uncertainty about eligibility ([reference](https://forum.csdms.io/t/docker-licensing-change/75)).
   In practice, many organizations and individuals are not eligible for free use. Docker on Windows via WSL2 also frequently introduces technical issues, and hardware (serial/USB) support is severely limited. Podman is slightly lighter but shares the same fundamental limitations.

If you *must* use containers, in an Ubuntu/Debian WSL2 environment:

.. code-block:: bash

   curl -L docker.iotempower.us | bash -
   # or for podman
   curl -L podman.iotempower.us | bash -

Be aware: **Serial device access is not possible under Windows containers.** Only network flashing (RFC2217) is available. For hardware device support, always use classic Linux or WSL2-native methods above.

Manual Installation
-------------------

If you do not trust the curl script, clone and manually inspect/run:

.. code-block:: bash

   cd # go to home directory
   git clone https://github.com/iotempire/iotempower iot
   cd iot
   bash run

Raspberry Pi (Deprecated)
-------------------------

.. warning::
   **Raspberry Pi images and installation are deprecated.**

   The preferred, best-supported method for IoTempower is a maintained, standard Linux PC or VM. Raspberry Pi images will no longer be maintained and are not recommended for new deployments.
   Gateway/infrastructure setups: see Manjaro below or use another mainstream Linux distribution.

Manjaro gateway setup
---------------------

Manjaro (KDE Plasma Desktop) continues to be a good option for classroom gateway devices and older/low-powered laptops. Successfully used in University of Tartu IoT courses, Spring 2024. See further instructions in this repository.

Installation, using existing router
-----------------------------------

You do not have to activate the gateway function—IoTempower is fully usable locally on desktops or servers.
Frequently, the GL.Inet Mango MT300 v2 is used as a class router.

Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
