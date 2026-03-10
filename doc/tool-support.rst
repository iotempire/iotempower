Tool Support
============

*IoTempower* features several scripted tools to simplify IoT development tasks.

**Use** ``<tool> help`` **to access helpful information about using any of these tools.**

The tools are available after starting the ``iot`` command (or ``run`` in the install directory).

Core Environment and Help
-------------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``iot``
     - Start an IoTempower shell environment, or run one command directly.
   * - ``iot exec <command>`` (or ``iot x``)
     - Execute a command inside the IoTempower environment, e.g. ``iot exec deploy``.
   * - ``iot menu`` / ``umenu``
     - Open the text-based menu interface with common workflows.
   * - ``iot help [cmd]`` / ``uhelp [cmd]``
     - Show command help and manual pages.
   * - ``iot env [ignore_system|ignore_node]``
     - Print resolved IoTempower environment variables from current config context.

Node and Firmware Workflow
--------------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``compile``
     - Compile firmware for the current node (or for all nodes below a system directory).
   * - ``deploy [node-address|compile|adopt|serial]``
     - Build and deploy firmware OTA, compile-only, or flash serial/adopt targets.
   * - ``initialize`` / ``adopt``
     - Wrapper for ``deploy adopt`` to initialize a serial node or adopt a node via dongle.
   * - ``console_serial [port] [password]``
     - Open a serial monitor for debug output from a locally connected node.
   * - ``create_node_template [name]``
     - Create a new node template in the current IoT system.
   * - ``create_system_template [name]``
     - Create a new IoT system template (from within ``iot-systems``).

Gateway Services and Access Point
---------------------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``iot service <start|stop|restart|status|view>``
     - Orchestrate web, MQTT, and access point services (tmux mode by default, direct mode with ``--no-tmux``).
   * - ``iot_service_web <start|stop|status> [full]``
     - Start/stop/status for web stack services.
   * - ``iot_service_mqtt <start|stop|status> [interfaces...]``
     - Start/stop/status for local MQTT broker service.
   * - ``iot_service_ap <start|stop|status> [options]``
     - Start/stop/status for access point lifecycle.
   * - ``accesspoint [show] [--wlan ...] [--gateway ...] [--wlan2 ...] [--gateway2 ...]``
     - Start an AP using configured defaults or interface/IP overrides (requires root privileges).

MQTT Tools
----------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``mqtt_broker`` / ``mqtt_start``
     - Start a local Mosquitto broker, optionally on one or two interfaces/IPs.
   * - ``mqtt_stop``
     - Stop the local MQTT broker process.
   * - ``mqtt_listen [topic_or_subtopic]``
     - Subscribe and print MQTT messages (auto-prefixes node topic in node directories).
   * - ``mqtt_send topic_or_subtopic message``
     - Publish a payload to MQTT (auto-prefixes node topic in node directories).
   * - ``mqtt_log [topic_or_subtopic] outfile``
     - Subscribe and append timestamped MQTT messages to a logfile.
   * - ``mqtt_action topic trigger_type trigger_data command``
     - React to MQTT messages and execute commands on trigger conditions.
   * - ``mqtt_generate_certificates``
     - Generate CA/server certificates and key material for MQTT TLS.

Documentation, Testing, and Maintenance
---------------------------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``iot doc`` / ``iot doc make``
     - Build manpages and HTML documentation.
   * - ``iot doc serve``
     - Build docs and serve them locally (default port: ``8000``).
   * - ``iot test [compile|deploy|hardware|install|smoketest|simple|<device>]``
     - Run test subsets (pytest-based) for compilation, deployment, hardware, or installation checks.
   * - ``iot install [clean]``
     - Install or reinstall the IoTempower environment and dependencies.
   * - ``iot upgrade``
     - Update IoTempower from git, refresh scripts, and rebuild docs.
   * - ``iot pio ...`` / ``iot platformio ...``
     - Run PlatformIO through IoTempower's managed Python environment.

Dongle and Advanced Utilities
-----------------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Command
     - Description
   * - ``dongle <command> [options]``
     - Manage adoption dongle actions (for example daemon, flash, scan, probe, display, adopt).
   * - ``iot server [args]``
     - Start the IoTempower server process relative to the active installation.


Top: `ToC <index-doc.rst>`_, Previous: `Supported Hardware <hardware.rst>`_,
Next: `Installation <installation.rst>`_
