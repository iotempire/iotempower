System Description
==================

Simple example to demonstrate subscribing and publishing MQTT topics with integriot.

- The button on the *buttons* node sends the messages "up" and "down"
- Integriot processes the button state, toggles an internal state every time the button is pressed
- Every time the internal state changes, integriot sends a message to the led on the *leds* node to switch it on or off


Connected nodes
---------------

- button
 - one button (button1) is connected with this node


- leds
 - one led (onboardled) is used


Super Systems
-------------
