Tool support
------------

*IoTempower* includes a lot of small scripts to simplify a plethora of tasks
supporting the deployment of a small IoT system or
`MQTT <http://mqtt.org/>`__-based IoT environment.
Among them are the following (most of these scripts are available after
starting the iot command or executing run in the main IoTempower directory):

- run/iot: Enter or run a command in the IoTempower-environment (setting
  system variables and virtual python environments). To run an IoTempower command, prefix
  it by the exec keyword, i.e.: ``iot exec accesspoint``. Make sure the iot script is
  installed in your bin-folder correctly (eventually check 
  `here </doc/installation.rst#installation-on-linux-and-wsl>`__, Point 4 again).

- ``iot menu``: Bundling most of the frequently used IoTempower tools
  under one convenient text based menu. Just try it out, but still look at
  the description below.

- ``accesspoint``:Start an accesspoint on a free WiFi interface (this is
  done by default on the IoTempower Raspberry Pi image). If you are running in Linux on a laptop and are connected to Ethernet
  but not WiFi, you have a good chance to turn your built-in WiFi here into an accesspoint.
  Very nice feature for teaching environments. This command does ask though for root access.

- ``iot upgrade``: Get latest version of IoTempower
  (inside an existing version)

- ``iot install``: (Re-)Install the IoTempower environment
  (if you specify clean, it re-installs)

- ``console_serial``: Open a serial console to see debug output of a
  serially (locally) connected node.

- ``initialize``: Initialize a current node which is in reconfiguration mode or
  flashes a serially connected node and sets all initial configuration
  parameters like WiFi credentials and security keys
  (this is the same as the ``deploy serial`` command).

- ``adopt``: Use a connected special hardware dongle (a flashed Wemos D1 Mini)
  to wirelessly flash an old IoTempower microcontroller that was in a different networks
  without attaching it to your PC/raspberry pi. More `here </doc/adopting.rst>`__.
  (This is the same as the ``deploy adopt`` command.)

- ``deploy``: updates software of a IoTempower node after changes. The update
  is done over the network (OTA).

- ``mqtt_broker``: Start a local mqtt broker (mosquitto) listening on all interfaces.

- ``mqtt_listen``: Subscribe to a given topic (or the node topic of the current node directory)
  deriving all configuration data from the current environment.

- ``mqtt_send``: Publish to a given topic (or the node topic of the current node directory)
  deriving all configuration data from the current environment.

- ``iot doc serve``: Run a documentation webserver on http://localhost:8001
  (also compiles the documentation first).

Most of these commands can be called with help (ie. ``deploy help``) to get more information.

Top: `ToC <index-doc.rst>`_, Previous: `Supported Hardware <hardware.rst>`_,
Next: `Installation <installation.rst>`_