Deep Sleep
==========

..  code-block:: cpp

    deep_sleep(time_from_now_ms, /* duration_ms */ 0);


Fall in ``time_from_now_ms`` ms into deep sleep mode for ``duration_ms`` ms.
Remember to wire ``RST`` to ``D0`` (or internal pin nr 16).
If duration_ms is 0 (default) the microcontroller can only be woken up
by the reset line.
``deep_sleep`` has to be specified in the start method.

**Careful:** any status of an actor is forgotten. Also the OTA update functionality
(and deploy) only work when the device is woken up and currently not sleeping.

Example
=======

..  code-block:: cpp

    void start() {
        // here can be something like device initialization or the transmission call
        deep_sleep(10000, 30000);
        // other device initializations
    }

This makes the node fall asleep in 10s and wake then up in another 30s.
