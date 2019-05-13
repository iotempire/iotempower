Supported Hardware
------------------

*iotempower* is targeted to run on a variety of (mainly Linux-based) hardware and
on wireless microcontrollers (initially mainly esp8266-based microcontrollers
and single-board Linux computers like the Raspberry Pi 3 or 
Raspberry Pi Zero W).

If you are interested in shopping for related hardware, check
http://iot.ulno.net/hardware
or go directly to AliExpress, Amazon, AdaFruit or Sparkfun and search for Wemos
D1 Mini, ESP8266, NodeMCU, 37-in-1 Arduino sensor kit.

The gateway services have been tested to run on:

- Raspberry Pi 1 (B and B+), 2, 3, and Zero W - however,
  the Raspberry Pi 3 is our first choice
- Laptops running Ubuntu Linux 17.04 and 18.04

We are trying to provide virtualbox images and cocker configurations
as soon as we find time
and/or volunteers.

We are also working on verifying that IoTempower works well on Orange-Pi Zero to
allow more cost-effective solutions to use iotempower.

Currently the following esp8266-based devices are supported:

- Wemos D1 Mini
- NodeMCU
- ESP-M1
- Espresso Lite V2
- Sonoff and Sonoff Touch
- There is an esp8266 generic option for other esp8266-based boards.
- We expect to support esp32 boards very soon - let us know if you want to help
  making this possible.

The part of IoTempower running on the esp8266 is a standalone C++-based firmware
managed by `PlatformIO <http://platform.io>`__. However, IoTempower abstracts a
lot of the burden of repetitive device management away from the user so that
attaching a device to a node usually boils down to just writing one line of
code, which you can adapt from plenty of examples.

Earlier versions were based on `micropython <http://www.micropython.org/>`__,
however, porting some of the C++-based Arduino device driver
libraries, managing 
remote access, updates, dealing with very little memory, and a slightly defunct
community, made management very hard leading us to the decision to switch to an
admittedly harder to program environment, however,
we earned the access to the huge
and active Arduino community making problem solving and extensions
much easier. We
do not regret the switch.

There was some initial effort in creating a starter development kit for
the Wemos D1 Mini - you can see more information `here
</doc/shields/wemosd1mini/devkit1/README.rst>`__. However, we are now more
focusing on using cheap hardware from various 37 in 1 sensor kits, which can
still easily be plugged together.

Top: `ToC <index-doc.rst>`_, Previous: `Architecture <architecture.rst>`_,
Next: `Tool Support <tool-support.rst>`_