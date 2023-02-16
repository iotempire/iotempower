System Description
==================

Simple example to demonstrate subscribing and publishing MQTT topics with iotknit.

- The button on the *button* node sends the messages "up" and "down"
- iotknit processes the button state, toggles an internal state every time the button is pressed
- Every time the internal state changes, iotknit sends a message to the led on the *led* node to switch it on or off


Connected nodes
---------------

- button
 - one button (button1) is connected with this node


- leds
 - one led (onboardled) is used


Super Systems
-------------
