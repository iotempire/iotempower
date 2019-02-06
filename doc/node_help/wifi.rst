adoption/reconfiguration
========================

To reconfigure wifi of a previously used ulnoiot node (if the node never has 
been flashed with ulnoiot before, use initialize serial to flash it), make sure
you have a flash button connected (usually D3 or pin 0 against ground).
Reset (or power off and on the node), wait until it starts blinking and press 
the flash button 2 or 3 times during this first blinking five seconds.

Then, the node is in adoption or reconfiguration mode, 
and starts its own WiFi accesspoint.
It will show up in the list of WiFi networks called something like
``ulnoiot-ab-mn`` (``ab``, ``mn`` vary - `m` is the number of long blinks,
``n`` the number of short 
blinks). The onboard-led will blink in some kind of Morse code (several long
blinks and several short blinks). Count the blinks - you can use them to verify
your node.

Use a phone or tablet (a computer works too) to connect to this wifi network.
The network is open and requires no password.
Modern phones will directly go to the configuration page or ask you to sign in
and then go to the configuration page. If this is not the case, try to manually
access the configuration page on http://192.168.4.1

Connect your node to your local WiFi or the ulnoiot Raspberry Pi gateway WiFi
(often called iotempire-something, also password iotempire).
Make sure to verify the
Morse code like blink pattern.

After you saved the configuration, you can navigate to your node-directory and
call: 

``initialize`` (or you call adoption/intialize from the
usermenu/``ulnoiot shell``)

This sends all security credentials and configuration values to your node.

For later updates, you just need to call ``deploy`` from your node directory.
