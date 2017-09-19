Node Description
================

Configuration for a portable water sensor demonstration. Connects to an
Android phone running a wifi accesspoint and ulnoiot in a termux environment.
The whole setup consists of an Android phone, a wemos d1 mini in a sealed
tupper-ware (with a whole and an external water sensor), a battery,
and a glass of water (or any other liquid to dip the water sensor into).

Connected devices
-----------------

Simple analog water sensor is connected to analog port. Also the blue onboard
led of the wemos d1 mini is used to show detection status.

Functionality
-------------

Water detection is communicated via mqtt to the mqtt-broker running on the
Android phone in ulnoiot in termux. Another server reacts on the Android device
to the raised detection signal.
