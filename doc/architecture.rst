Architecture
------------

.. figure:: /doc/images/system-architecture.png
   :width: 50%
   :figwidth: 100%
   :align: center
   :alt: IoTempower System Architecture

   IoTempower System Architecture

In an IoTempower system you find one or several IoTempower gateways and wirelessly
connected nodes with physically connected devices (sensors or actors).

An *IoTempower gateway* contains and runs all the software needed to configure an
IoT system. It also provides facilities to run and manage a WiFi router as well
as an MQTT-broker. It therefore provides configuration management software as
well as dataflow management services.

Such a gateway can for example easily be installed on a Raspberry Pi 3.
*Nodes* are wireless components which interact with physical objects. Usually a
*node* has several *devices* attached to it.
*devices* can be sensors (like a button or
temperature, light, movement, or humidity sensor)
or actors (like relais, solenoids,
motors, or leds).

Top: `ToC <index-doc.rst>`_, Previous: `Introduction <introduction.rst>`_,
Next: `Supported Hardware <hardware.rst>`_