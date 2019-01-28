=======
ulnoiot
=======

Attention: this is ulnoiot-tng (The Next Generation, not anymore based on 
micropython, now using `PlatformIO <platform.io>`__ as a base to generate
firmware and code).


Introduction
------------

*ulnoiot* (pronounced: "You'll know IoT") is a framework and environment
for making it easy for everyone to explore and develop for the
Internet of Things (IoT)
-- easy for tinkerers, makers, programmers, hobbyists, students, artists,
and professionals alike.
It has a special focus on education and is intended to support classes to teach
Internet of Things (IoT) and
home automation.

However, it also supports existing IoT deployments and brings
mechanisms for over the air (OTA) updates and automatic
multi-device deployment. 

If you are impatient and want to dive into it right now, fast forward to
`Installation`_ or `First IoT Nodes`_.

ulnoiot is based on a multi-layered network architecture. This means for this project
that each IoT-system (small network of connected sensors and actors) has its own
gateway - usually running an MQTT-broker. These gateways can be connected to cloud
resources, other cloud or Internet based MQTT brokers or interconnected among
themselves. Security and privacy can be selectively controlled at each layer
border (everywhere, where a gateway connects two layers).
In a teaching context these gateways are usally based on a modified Raspbian
running on a Raspberry Pi of a
newer generation (wifi on board - both the Raspberry Pi 3 and the Raspberry Pi Zero W
can be easily used as an ulnoiot gateway at this point)
to allow them to work as wifi routers.

We are currently rewriting the documenation.

`The old readme is here </doc/README-old.rst>`_.