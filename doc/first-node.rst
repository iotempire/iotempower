==========
First Node
==========


If you are using the pi-image, you can find a short walkthrough of this here: 
https://youtu.be/fTWNYXfet9E

For more video instructions, check the `IoTempower YouTube Video Playlist </https://www.youtube.com/playlist?list=PLlppUpfgGsvkfAGJ38_mzQc1-_Z7bNOgq>`_.


For this section, we assume that you have IoTempower successfully set-up in your
Linux environment (as described 
`here </doc/installation.rst#installation-on-linux-and-wsl>`_) and that you can successfully
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
and our first node should be called "test01". (The pi image has such a folder
setup in iot-systems. In older versions of the pi-image though there are some
old variables in the ``system.conf``. Pay attention to its content below.)

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

If you have the pi-image version or cloudcmd installed or web_starter is running, you can alternatively
navigate to your `gateway's system folder </cloudcmd/fs/home/iot/iot-systems/test01>`_
via cloudcmd. Tap F2 there and create a new node via that menu and then rename
that folder to test01.

You can, of course, use another system name than "demo" (and even move
or rename this folder later - on the pi image it's called also test01). Renaming nodes at a later point
requires a re-flash of the corresponding microcontroller, so let's be a
bit more careful here.

If you have the pi-image version, you can skip all the networking steps
below as it takes care of all the network setup for you.

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
uncomment and adjust
the lines for ``IOTEMPOWER_MQTT_HOST``, (if you use cloudcmd,
you can use the built-in editor, but if you are actually on cloudcmd on the pi-image,
you can skip this step as the defaults will be correct):

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
     iot doc serve # if your are running web_starter, you can just look at the webpage on port 40080

  If both of these commands worked, you should now be able to go to 
  http://localhost:8001 in your browser and see some documentation
  and also be able to read up on the ``input``-command.


First Deployment
================

1. Now, prepare the Wemos: attach the Button Shield to your Wemos D1 Mini.
   Make sure that the pins align with the Wemos
   (on other microcontrollers make sure you have some button connected
   and know the corresponding GPIO port).

2. In the ``test01`` (and inside the iot environment) folder,
   run the following command

   In Linux (if you run in a virtual machine, pass through your serial USB
   of your microcontroller): ``deploy serial`` (or select from cloudcmd menu)

   In Windows:

   - In Windows using WSL 2 or in docker, you currently can only remote flash.
     You can just run ``compile`` instead of ``deploy serial`` and then download
     the firmware from your build folder and flash it manually.
   
   - You can also setup net2ser on your router.
     Run the following command, but replace the X with the number after COM (in the above example X = 8)

     And, even more exciting, you can now flash devices (i.e. Wemos D1 Mini) 
     that are connected to your openwrt router with usb port (like the Mango).
     You need to install ser2net, luci-app-ser2net, kmod-usb-serial-ftdi, 
     kmod-usb-serial-cp210x,  kmod-usb-serial-ch341,  kmod-usb-serial-pl2303 
     on the openwrt router. 
     Then configure ser2net to allow rfc2217 on port 2000 as well as add a 
     proxy on port 5000 with the right port (for wemos d1 mini /dev/ttyUSB0)
     and rfc2217 and cts/dts enabled (default port settings 115000 baud 8n1).

     Then you need to connect the device in force-flash-mode (on d1 mini: connect gnd and d3).
     Now, you can flash with IoTempower using ``deploy serial rfc2217://192.168.14.1:5000``
     or from platformio with the follwing entries in your platformio.ini:

     .. code-block:: ini

        [env:your_environment]
        platform = espressif8266
        board = nodemcuv2
        framework = arduino

        monitor_port = rfc2217://192.168.14.1:5000
        monitor_speed = 115200

        upload_port = rfc2217://192.168.14.1:5000
        upload_speed = 115200


     Assuming your router's local ip address is 192.168.14.1
     (if you want the serial monitor, you need to remove the force-flash wire of course)

3. Open 2 more Linux terminals (make sure you are in iot environment for all 3 terminals).
   Run a singular command on one terminal, like the following
   
   Terminal 1: ``console_serial`` (in Linux) or in Windows you can 
   use the Arduino-IDE serial console

   Terminal 2: ``mqtt_listen``

   (just for confirmation, if mqtt needs to run locally,
   there should be one terminal running ``mqtt_broker``)
   If you have trouble stopping ``console_serial`` with Ctrl-C,
   you can also kill it from another terminal with ``pkill -f monitor``.

4. Now press the button that is attached to your Wemos D1 Mini.
   If you did everything correctly, 
   you should see "released" , "pressed", on the terminal
   where you ran ``mqtt_listen``
   as well as lots of debug information and also 
   ``pressed`` and ``released`` on the terminal
   running ``console_serial``.

In advanced settings, you can extend your installation with
a specially flashed Wemos D1 Mini, called a dongle allowing you to
to flash previously flashed (or preflashed) esp8266 and esp32 based
microcontrollers without them being registered in your WiFi (you
can put any microcontroller flashed with IoTempower in a
so-called adoption mode. For more look here: `Adoption <adopting.rst>`__
If your serial flashing works and you don't have lots of devices to manage,
adoption might be only of marginal interest.
So only use it when you have gathered some experience with IoTempower.
The dongle usually makes the most sense on raspberry pi IoTempower installations.
You can probably ignore it if you have a installtion on a dedicated PC or
in a local environment.

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
