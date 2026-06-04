virtual
=======

The ``virtual`` device lets you build software-defined devices that can:

- receive MQTT commands,
- emit status/value reports to MQTT,
- run a timed loop (similar to animator frames).

Overview
--------

Use ``virtual`` when you want custom device behavior without direct hardware I/O.
You define command handlers and optional loop logic directly in ``setup.cpp``.

Syntax
------

.. code-block:: cpp

    virtual(name)
        .with_fps(frames_per_second)
        .with_loop(loop_function)
        .with_command_handler("command_name", handler_function)
        .report("status");

Main API
--------

- ``with_command_handler("cmd", [] (Ustring& command) { ... })``
  Register a command handler. If ``""`` is used, it acts as a fallback.
- ``with_loop([] { ... })``
  Called in a timed loop based on FPS.
- ``with_frame_builder([] { ... })``
  Alias for ``with_loop`` (animator-style naming).
- ``with_fps(n)`` / ``fps(n)``
  Configure loop frequency (1..100).
- ``report(...)``
  Publish a new status/value (supports strings and integers).

Simple Example: Second Counter with Reset Command
-------------------------------------------------

.. code-block:: cpp

    // setup.cpp

    unsigned long seconds_passed = 0;

    virtual(counter)
        .with_fps(1)
        .with_loop([] {
            seconds_passed++;
            IN(counter).report(seconds_passed);
        })
        .with_command_handler("reset", [] (Ustring& command) {
            seconds_passed = 0;
            IN(counter).report(seconds_passed);
        });

MQTT Usage
----------

Assuming your node topic is ``home/lab/node1`` and the device name is ``counter``:

- Status/value topic:

  .. code-block:: bash

      home/lab/node1/counter

- Command topic:

  .. code-block:: bash

      home/lab/node1/counter/set

Send reset command:

.. code-block:: bash

    mosquitto_pub -t home/lab/node1/counter/set -m "reset"

Read counter updates:

.. code-block:: bash

    mosquitto_sub -t home/lab/node1/counter
