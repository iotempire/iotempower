Tool support
------------

*ulnoiot* includes a lot of small scripts to simplify a plethora of tasks
supporting the deployment of a small IoT system or
`MQTT <http://mqtt.org/>`__-based IoT environment.
Among them are the following (most of these scripts are available after
starting the ulnoiot command or executing run in the main ulnoiot directory):

- run/ulnoiot: enter or run a command in the ulnoiot-environment (setting
  system variables and virtual python environments). To run a command, prefix
  it by the exec keyword, i.e.: ``ulnoiot exec accesspoint``

- accesspoint: start an accesspoint on a free WiFi interface

- ulnoiot upgrade: get latest version of ulnoiot (inside an existing version)

- ``ulnoiot_install`` or ulnoiot install: (re-)install the ulnoiot environment
  (if you specify clean, it re-installs)

- ``console_serial``: open a serial console to see debug output of a
  serially (locally) connected node

- ``initialize``: initialize a current node which is in reconfiguration mode or
  flashes a serially connected node and sets all initial configuration
  parameters like WiFi credentials and security keys

- ``deploy``: updates software of a ulnoiot node after changes. The update
  is done over the network (OTA)

Top: `ToC <index-doc.rst>`_, Previous: `Supported Hardware <hardware.rst>`_,
Next: `Installation <installation.rst>`_