Architecture
------------

.. figure:: /doc/images/system-architecture.png
   :width: 50%
   :figwidth: 100%
   :align: center
   :alt: IoTempower System Architecture

   IoTempower System Architecture

In an IoTempower system you find one or several IoTempower gateways (the edge)
and wirelessly connected nodes - usually microcontrollers like the Wemos D1 mini
or Nodemcu. This microcontrollers have physically connected devices: sensors or actors
(also called things in an Internet of Things architecture).

An *IoTempower gateway* contains and runs all the software needed to configure an
IoT system. It also provides facilities to run and manage a WiFi router as well
as an MQTT-broker. It therefore provides configuration management software as
well as dataflow management services. However, if you can run IoTempower also
on your own dedicated Linux system (or even on WSL 1) and have other gateway
services separately. So you could have a wifi router providing your 
IoT edge network and an MQTT-broker (like mosquitto) running on this router,
another dedicated Linux installation, or inside your dedicated Linux system.conf
Depending on your choice, you therefore might separate WiFi router, broker,
and deployment. Your integration logic like 
`Node-RED <https://nodered.org/>`__ or 
`Home-Assistant <https://www.home-assistant.io/>`__
might run on another dedicated Linux installation or on any of the
existing.

An integrated gateway but also any of the single services can for example easily be installed on a Raspberry Pi 3 or 4.
*Nodes* are wireless components which interact with physical objects (things).
Usually a *node* has several *devices* (things) attached to it.
Do not confuse these hardware nodes with the nodes of a flow in Node-RED.
*devices* can be sensors (like a button or
temperature, light, movement, or humidity sensor)
or actors (like relais, solenoids,
motors, or LEDs).


Top: `ToC <index-doc.rst>`_, Previous: `Introduction <introduction.rst>`_,
Next: `Supported Hardware <hardware.rst>`_