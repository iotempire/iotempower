==========
First Node
==========

For this section, we assume that you have IoTempower successfully set-up in your
a Linux environment (as described 
`here </doc/installation.rst#installation-on-linux-and-wsl>`_) and that you can successfully
enter the IoTempower environment (prompt changes to IoT).

We will now set-up and configure our first IoT node (if you want to know
what a *node* is, check the `architecture chapter <architecture.rst>`_).

IoTempower supports among others the following microcontroller boards
(for more supported boards, check the `hardware chapter <hardware.rst>`_):

- Wemos D1 Mini Mini
- NodeMCU
- Espresso Lite V2
- esp32 webkit mini

Creating an IoTempower System Folder
====================================

To install IoTempower to one of these microcontroller boards,
you need to setup a folder describing your IoT system (the
gateway services, the nodes, and the connected devices/things).

The top folder describes the system and has a ``system.conf``
configuration file (we will call it therefore `system folder`).
In addition to the ``system.conf`` file the
folder includes the locations and sub-locations (sub folders) of
the nodes networked into the IoT system. This allows to model a
hierarchical nodes and connected things architecture for having
something like ``ulnos-home-system/living-room/dining-corner/switches``
and in there a ``node.conf`` and ``setup.cpp`` file describing a
sonoff 3 channel switch and potential gestures on it (see also
/examples/sonoff/t1-3).

To create such a system folder, we need to copy one of the folders
of the cloned git repository to the
outside of the repository. Let's assume, the new system is called "demo"
and our first node should be called "test01".

To create the initial folder, do the following:

.. code-block:: bash

  mkdir ~/iot-systems
  cp -R "$IOTEMPOWER_ROOT/lib/system_template" ~/iot-systems/demo
  cd ~/iot-systems/demo
  mv node-template test01

You can of course use another system name than "demo" (and even move
or rename this folder later). Renaming nodes at a later point
requires a re-flash of the corresponding microcontroller, so let's be a
bit more careful here.

Depending on your network setup, we now need to figure out some ipconfig
addresses and configure things accordingly to make sure our node can
connect to the right services.

For WSL 1 in Windows

- Open the Command Prompt
- Run ``ipconfig``
- Look for and note down ``IPv4 Address`` and ``Default Gateway IP`` 

For Linux

- Open Terminal
- Run ``ip a``
- Look for and note down ``inet`` <address of interest> ``scope global dynamic ip``
- Run ``ip r``
- Look for and note down ``default gateway via`` <address of interest>.

If you have an mqtt broker like mosquitto running in your local network,
find and note down its IP address and ignore the next indented text block.

  If you don't have an mqtt broker running ...

  Enter iot environment (with for example ``iot`` command).

  Create a global iot configuration for this Linux installation
  by copying ``etc/iotempower.example.conf`` to to just iotempower.conf:

  ``cp "$IOTEMPOWER_ROOT/etc/iotempower.conf.example" "$IOTEMPOWER_ROOT/iotempower/etc/iotempower.conf"``

  Set and adjust IOTEMPOWER_MQTT_HOST of the iotempower.conf to the following (note that {values} need to changed to what you found in the above steps or to whatever your SSID is)
  ``nano "$IOTEMPOWER_ROOT/etc/iotempower.conf"`` - don't forget to uncomment this line (remove leading #),
  make sure that there are no blanks before or after the equal sign.

  .. code-block:: bash

     IOTEMPOWER_MQTT_HOST={IPv4 IP from above}

  This needs to be updated, each time your Linux IP address changes.

  Leave the iot environment (run ``exit``).

  You can (and should) now run a local mqtt broker (mosquitto) by running
  ``iot exec mqtt_broker``. This will block this terminal
  but give some nice logging output.
  You can interrupt it with pressing Ctrl-C twice or issuing
  ``pkill mqtt_broker; pkill mosquitto`` from another terminal.

  If the IP address didn't change you can also start the mqtt broker
  now at any time with ``iot exec mqtt_broker``.

Go to the folder we copied earlier:
   
``cd ~/iot-systems/demo``

Edit the system.conf file (``nano system.conf``), uncomment and adjust
the lines for ``IOTEMPOWER_MQTT_HOST``, :

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

   input(button1, D3, "released", "pressed");

..

  (Optional, Recommended) You can see some documentation for more information by running the following commands
  in another terminal.

  .. code-block:: cpp

     # iot doc make  # is included in the next
     iot doc serve

  If both of these commands worked, you should now be able to go to 
  http://localhost:8001 in your browser and see some documentation
  and also be able to read up on the ``input``-command.


First Deployment
================

1. Now, prepare the Wemos: attach the Button Shield to your Wemos D1 Mini.
   Make sure that the pins align with the Wemos
   (on other microcontroller make sure you have some button connected
   and know the corresponding GPIO port).

2. In the ``test01`` (and inside the iot environment) folder,
   run the following command

   In Linux (if you run in a virtual machine, pass through your serial USB
   of your microcontroller): ``deploy serial``

   In Windows: 
   
   - Take note of the COM port number that the `Device Manager`
     shows under `Ports` (for example (COM8) for the connected Wemos D1 Mini
     (this particular microcontroller usually shows as USB-SERIAL CH340 (COMy)
     with y some kind of integer number) as seen in this screenshot.

     .. figure:: images/windows-serial-ports.png
        :width: 70%
        :figwidth: 100%
        :align: center
        :alt: Serial port enumeration in Windows 10 - showing 8 for connected Wemos D1 Mini

   - Run the following command, but replace the y with the number after COM (in the above example y = 8)
     
     ``deploy serial ttySy``

3. Open 2 more Linux terminals (make sure you are in iot environment for all 3 terminals).
   Run a singular command on one terminal, like the following
   
   Terminal 1: ``console_serial ttySy``

   Terminal 2: ``mqtt_listen``

   (just for confirmation, if mqtt needs to run locally,
   there should be one terminal running ``mqtt_broker``)

4. Now press the button that is attached to your Wemos D1 Mini.
   If you did everything correctly, 
   you should see "released" , "pressed", on the terminal
   where you ran ``mqtt_listen``
   as well as lots of debug information and also 
   ``pressed`` and ``released`` on the terminal
   running ``console_serial``.

In advanced settings, you can extend your installation with_debounce
a specially flashed Wemos D1 Mini, called a dongle allowing you to
to flash previously flashed (or preflashed) esp8266 and esp32 based
microcontrollers without them being registered in your WiFi (you
can put any microcontroller flashed with IoTempower in a
so-called adoption mode. For more look here: `Adoption <adopting.rst>`__


Next deployments
================

From now on, the software of your node can be updated with a simple ``deploy``
issued inside the node folder.

Edit ``setup.cpp`` and add the onboard led under your button like this (and debounce the button):

.. code-block:: cpp
   
   input(button1, D3, "released", "pressed").with_debounce(5);
   output(blue, ONBOARDLED).inverted();

Connect your wemos to another power supply that is not connected to your
computer (or at least reset it once by pressing the button)
- wait 10s to make sure it's connected to WiFi and mqtt again.

Run ``deploy``

Congratulations!! Your node is now setup and connected with IoTempower.
Try to react to some button presses in Node-RED subscribing to
``test01/button1`` or changing the onboard led sending on or off to
``test01/blue/set``.


Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `Second Node <second-node.rst>`_.
`Versão em português aqui <first-node-pt.rst>`_.
