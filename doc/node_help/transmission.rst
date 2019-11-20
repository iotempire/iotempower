transmission
============

..  code-block:: cpp

    transmission_interval(transmission_interval=5)

Set the status report transmission interval (must be placed into the
start method, see running-node-test example).

Parameter
---------

- ``transmission_interval``: send every transmission_interval seconds a complete
  status update. If ``transmission_interval=0``, send never status updates.

example
-------

..  code-block:: cpp

    void start() {
        transmission_interval(10)
    }
