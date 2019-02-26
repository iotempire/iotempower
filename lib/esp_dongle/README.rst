==========
ESP Dongle
==========

This project is a dongle that should be connected to a ulnoiot gateway
(like a raspberry pi with the ulnoiot pi image).

It should be installed to Wemos D1 Mini with display shield.

It offers a small serial command line to do the following things:

- display information on the local display
- button access (to the two buttons of the shield)
- connecting to a given WiFI network and then sending an OTA firmware to a
  given IP on that network
- in bin are several scripts that handle communication with
  such a dongle
- this might also an ESP-Now gateway at a later point
- at a later point, we might also connect LORA to this gateway to
  feed it back into mqtt on the pi
