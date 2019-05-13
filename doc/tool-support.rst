Tool support
------------

*IoTempower* includes a lot of small scripts to simplify a plethora of tasks
supporting the deployment of a small IoT system or
`MQTT <http://mqtt.org/>`__-based IoT environment.
Among them are the following (most of these scripts are available after
starting the iot command or executing run in the main IoTempower directory):

- run/iot: enter or run a command in the IoTempower-environment (setting
  system variables and virtual python environments). To run a command, prefix
  it by the exec keyword, i.e.: ``iot exec accesspoint``

- ``iot menu``: Bundling most of the frequently used IoTempower tools
  under one convenient text based menu. Just try it out, but still look at
  the description below.

- accesspoint: start an accesspoint on a free WiFi interface (this is
  done by default on the IoTempower Raspberry Pi image). 

- ``iot upgrade``: get latest version of IoTempower
  (inside an existing version)

- ``iot install``: (re-)install the IoTempower environment
  (if you specify clean, it re-installs)

- ``console_serial``: open a serial console to see debug output of a
  serially (locally) connected node

- ``initialize``: initialize a current node which is in reconfiguration mode or
  flashes a serially connected node and sets all initial configuration
  parameters like WiFi credentials and security keys

- ``deploy``: updates software of a IoTempower node after changes. The update
  is done over the network (OTA)

Top: `ToC <index-doc.rst>`_, Previous: `Supported Hardware <hardware.rst>`_,
Next: `Installation <installation.rst>`_