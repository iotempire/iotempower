adoption/reconfiguration
========================

To reconfigure wifi of a previously used IoTempower node (if the node never has 
been flashed with IoTempower before, use deploy serial to flash it), make sure
you have a flash button connected (usually D3 or pin 0 against ground),
a small display shield often has butons too and might even be used to show something
status information during adoption.

Reset (or power off and then on again the node), wait until it starts blinking and press 
the flash button 2 or 3 times during this first blinking five seconds.

Then, the node is in adoption or reconfiguration mode, 
and starts its own WiFi accesspoint.
It will show up in the list of WiFi networks called something like
``uiot-ab-mn`` (``ab``, ``mn`` vary - `m` is the number of long blinks,
``n`` the number of short 
blinks). The onboard-led will blink in some kind of Morse code (several long
blinks and several short blinks). Count the blinks - you can use them to verify
your node.

Connect an IoTempower dongle to your computer or pi gateway and make sure the
dongle daemon is running and use usermenu/``iot menu`` and select ``adopt/initialize`` to initialize the node
(from a node folder).
Make sure to verify the Morse code like blink pattern.

This sends all security credentials and configuration values to your node.

For later updates, you just need to call ``deploy`` from your node directory.
