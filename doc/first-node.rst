=====================
First Node (Advanced)
=====================

.. note::
   **New to IoTempower?** Start with the `Quickstart Guide <quickstart.rst>`_ instead!
   This guide covers everything you need to get your first node running quickly.
   
   This advanced tutorial is for users who want detailed explanations of the underlying
   system and are comfortable with manual configuration steps.

For video instructions, check the `IoTempower YouTube Video Playlist <https://www.youtube.com/playlist?list=PLlppUpfgGsvkfAGJ38_mzQc1-_Z7bNOgq>`_.

For this section, we assume that you have IoTempower successfully set-up in your
Linux environment (as described 
`here <installation.rst#installation-on-linux-and-wsl>`_) and that you can successfully
enter the IoTempower environment (prompt changes to IoT).

We will now set-up and configure our first IoT node (if you want to know
what a *node* is, check the `architecture chapter <architecture.rst>`_).

IoTempower supports, among others, the following microcontroller boards
(for more supported boards, check the `hardware chapter <hardware.rst>`_):

- Wemos D1 Mini Mini
- NodeMCU
- Espresso Lite V2
- esp32 webkit mini
- M5StickC and M5StickC Plus (not yet Plus2)

Creating an IoTempower System Folder
====================================

.. note::
   The `Quickstart Guide <quickstart.rst>`_ provides a simpler approach using 
   ``create_system_template`` and ``create_node_template`` commands. This section 
   shows the manual approach for educational purposes.

To install IoTempower to one of these microcontroller boards,
you need to setup a folder describing your IoT system (the
gateway services, the nodes, and the connected devices/things).

The top folder describes the system and has a ``system.conf``
configuration file (we will call it, therefore `system folder`).
In addition to the ``system.conf`` file the
folder includes the locations and sub-locations (sub folders) of
the nodes networked into the IoT system. This allows to model
hierarchical nodes and a connected things architecture for having
something like ``ulnos-home-system/living-room/dining-corner/switches``
and in there a ``node.conf`` and ``setup.cpp`` file describing a
sonoff 3 channel switch and potential gestures on it (see also
/examples/sonoff/t1-3).

To create such a system folder, we need to copy one of the folders
of the cloned git repository to the
outside of the repository. Let's assume the new system is called "demo"
and our first node should be called "test01".

.. note::
   **Optional: Using the Raspberry Pi Image**
   
   If you're using the IoTempower Raspberry Pi image, there's a pre-configured 
   folder in ``~/iot-systems`` with example configurations. The pi image also 
   provides CloudCmd for web-based file management. However, the command-line 
   approach shown here works on all platforms.

To create the initial folder via the command-line, do the following:

.. code-block:: bash

  mkdir ~/iot-systems
  cp -R "$IOTEMPOWER_ROOT/lib/system_template" ~/iot-systems/demo
  cd ~/iot-systems/demo
  mv node_template test01

If you look at ``~/iot-systems/demo/system.conf``, you should see the following:

.. code-block:: bash

  # Configuration for a IoTempower system
  # Everything from etc/iotempower.conf can be overwritten here.
  # However, you will usually just overwrite the MQTT-Host and maybe
  # wifi access information

  # Name of accesspoint to connect to
  #IOTEMPOWER_AP_NAME="iotempire"

  # Its wifi password
  #IOTEMPOWER_AP_PASSWORD="internetofthings"

  # Connect data for local MQTT broker
  # If not defined, the same as IOTEMPOWERAP_IP from $IOTEMPWOER_ROOT/etc/iotempower.conf default
  #IOTEMPOWER_MQTT_HOST="mosquitto-host"
  
As you see, everything is commented out and does not require immediate attention,
but if you later want to take this folder to a different PC and deploy from there,
use the correct variable names to overwrite some settings (so better check
and eventually update now than being confused later).

.. note::
   You can also use the ``iot menu`` command (press F2) to create a new node via 
   the interactive menu system. This works on all platforms and is generally easier 
   than manual folder creation.

You can, of course, use another system name than "demo" (and even move
or rename this folder later). Renaming nodes at a later point
requires a re-flash of the corresponding microcontroller, so let's be a
bit more careful here.

Network Configuration
=====================

.. note::
   The `Quickstart Guide <quickstart.rst>`_ provides step-by-step instructions 
   for network setup on various platforms. Refer to it for the most current 
   and comprehensive guidance.

Depending on your network setup, we now need to figure out some ``ipconfig``
addresses and configure things accordingly to make sure our node can
connect to the right services.

For WSL 2 in Windows

- Open a powershell
- Run ``ipconfig``
- Look for and note down ``IPv4 Address`` and ``Default Gateway IP`` 

For Linux

- Open Terminal
- Run ``ip a``
- Look for and note down ``inet`` <address of interest> ``scope global dynamic ip``
- Run ``ip r``
- Look for and note down ``default gateway via`` <address of interest>.

If you have an mqtt broker like mosquitto or mqttsuite running in your local network,
find and note down its IP address and ignore the next indented text block.

  If you don't have an mqtt broker running ...

  Enter iot environment (with for example ``iot`` command).

  Create a global iot configuration for this Linux installation
  by copying ``etc/iotempower.example.conf`` to to just iotempower.conf:

  ``cp "$IOTEMPOWER_ROOT/etc/iotempower.conf.example" "$IOTEMPOWER_ROOT/etc/iotempower.conf"``

  Set and adjust IOTEMPOWER_MQTT_HOST of the iotempower.conf to the following (note that {values} need to changed to what you found in the above steps or to whatever your SSID is)
  ``nano "$IOTEMPOWER_ROOT/etc/iotempower.conf"`` - don't forget to uncomment this line (remove leading #),
  make sure that there are no blanks before or after the equal sign.

  .. code-block:: bash

     IOTEMPOWER_MQTT_HOST={IPv4 IP from above}

  This needs to be updated each time your Linux IP address changes.

  Leave the iot environment (run ``exit``).

  You can (and should) now run a local mqtt broker (mosquitto) by running
  ``iot exec mqtt_broker``. This will block this terminal
  but give some nice logging output.
  You can interrupt it with pressing Ctrl-C twice or issuing
  ``pkill mqtt_broker; pkill mosquitto`` from another terminal.

  If the IP address didn't change, you can also start the mqtt broker
  now at any time with ``iot exec mqtt_broker``.

  You can also try just running ``iot exec mqtt_broker scanif`` - 
  this might guess your local ip-address correctly.

Go to the folder we copied earlier:
   
``cd ~/iot-systems/demo``

Edit the system.conf file (``nano system.conf`` - or use your favorite editor),
uncomment and adjust the lines for ``IOTEMPOWER_MQTT_HOST``:

.. code-block:: bash

   IOTEMPOWER_AP_NAME="{wifi ssid}"
   IOTEMPOWER_AP_PASSWORD="{wifi password}"    
   IOTEMPOWER_MQTT_HOST={mqtt broker IP}

This has to be adjusted when using a different mqtt broker or wifi
(or if the local ip changed where the mqtt broker runs).

We will now write a small script for the Wemos D1 Mini
(this should also work for other supported microcontrollers,
but not board and GPIO ports might have to be adjusted).

``cd test01``

``nano setup.cpp``

Add the following line to the end of the setup.cpp file:

.. code-block:: cpp

   input(button1, D3, "released", "pressed").with_debounce(5);

.. note::
   For detailed documentation on available commands and device types, see the 
   `Command Reference <node_help/commands.rst>`_ or run ``iot doc serve`` to 
   start the local documentation server at http://localhost:8001.


First Deployment
================

1. Now, prepare the Wemos: attach the Button Shield to your Wemos D1 Mini.
   Make sure that the pins align with the Wemos
   (on other microcontrollers make sure you have some button connected
   and know the corresponding GPIO port).

2. In the ``test01`` folder (and inside the iot environment),
   run the deployment command.

   **On Linux:**
   
   .. code-block:: bash

      deploy serial

   If you run in a virtual machine, pass through your serial USB of your microcontroller.

   **On Windows (WSL2):**
   
   You need to use usbipd-win to access USB devices. See the 
   `Quickstart Guide <quickstart.rst#platform-specific-serial-flashing>`_ 
   for detailed instructions on using the WSL USB GUI or usbipd command line.

   **Network Flashing (Optional):**
   
   If you have an OpenWRT router with USB ports and ser2net configured,
   you can flash over the network:
   .. code-block:: bash

      deploy serial rfc2217://192.168.14.1:5000

   (Replace with your router's IP and configured port)

   For detailed ser2net setup instructions on OpenWRT routers, see the 
   `Quickstart Guide <quickstart.rst>`_.

3. Open 2 more terminals (make sure you are in iot environment for all terminals).
   Run these commands in separate terminals:
   
   Terminal 1: ``console_serial`` (or use the Arduino-IDE serial console on Windows)

   Terminal 2: ``mqtt_listen``

   (If running mqtt_broker locally, it should be running in another terminal)
   If you have trouble stopping ``console_serial`` with Ctrl-C,
   you can also kill it from another terminal with ``pkill -f monitor``.

4. Now press the button that is attached to your Wemos D1 Mini.
   If you did everything correctly, 
   you should see "released" and "pressed" messages on the terminal
   where you ran ``mqtt_listen``
   as well as lots of debug information and also 
   ``pressed`` and ``released`` on the terminal
   running ``console_serial``.

.. note::
   **Optional: Adoption Mode with a Dongle**
   
   In advanced settings, you can use a specially flashed Wemos D1 Mini called 
   a "dongle" to flash IoTempower to microcontrollers without connecting them 
   to your WiFi first. This is most useful when managing many devices or using 
   the Raspberry Pi image. For details, see `Adoption <adopting.rst>`_.
   
   For most users doing serial flashing on their local machine, adoption is 
   not necessary and can be safely ignored until you have more experience 
   with IoTempower.

Next deployments
================

From now on, the software of your node can be updated with a simple ``deploy``
issued inside the node folder (no need for ``serial`` - updates happen over WiFi).

Edit ``setup.cpp`` and add the onboard led under your button like this (and debounce the button):

.. code-block:: cpp
   
   input(button1, D3, "released", "pressed").with_debounce(5);
   output(blue, ONBOARDLED).inverted();

Connect your wemos to another power supply that is not connected to your
computer (or at least reset it once by pressing the reset button)
- wait 10s to make sure it's connected to WiFi and mqtt again.

Run ``deploy``

Congratulations!! Your node is now setup and connected with IoTempower.
Try to react to some button presses in Node-RED subscribing to
``test01/button1`` or changing the onboard led by sending "on" or "off" to
``test01/blue/set``.

For details on using Node-RED, see the `Quickstart Guide <quickstart.rst#controlling-devices-via-node-red>`_.


Top: `ToC <index-doc.rst>`_, Previous: `Quickstart <quickstart.rst>`_,
Next: `Second Node <second-node.rst>`_.
`Versão em português aqui <first-node-pt.rst>`_.
