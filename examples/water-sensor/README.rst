Node Description
================

Configuration for a portable water sensor demonstration. Connects to an
mqtt-server.
The whole setup consists of an ulnoiot gateway (this could be an Android
phone in termux with activated access point), a wemos d1 mini in a sealed
tupper-ware (with a whole and an external water sensor), a battery,
and a glass of water (or any other liquid to dip the water sensor into).

Connected devices
-----------------

Simple analog water sensor is connected to analog port. Also the blue onboard
led of the wemos d1 mini is used to show detection status.

Functionality
-------------

Water detection is communicated via mqtt to the mqtt-broker. Another server
(for example Node-RED) reacts to the raised detection signal.
