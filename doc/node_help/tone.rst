tone
====

..  code-block:: cpp

    tone(name, pin/*, pwm frequency is by default 1000*/);

Create a buzzer tone device based on PWM.

It subscribes to:

- ``[nodename]/[name]/play`` for melody strings

It publishes:

- ``[nodename]/[name]/status`` with ``playing`` and ``stopped``

Send ``stop`` to ``.../play`` to stop playback immediately.

Melody format
-------------

The tone device uses RTTTL-style strings:

``name:d=<duration>,o=<octave>,b=<bpm>,k=<key>:<notes>``

- ``d``: default note duration denominator (for example ``4`` = quarter note)
- ``o``: default octave
- ``b``: tempo in bpm
- ``k``: optional key with major/minor marker (for example ``cmaj``, ``am``, ``dmin``)
- ``notes``: comma-separated notes, for example ``8c,8d,8e,2p,8g#6``

Notes support:

- ``a``..``g`` and ``p`` (pause)
- optional ``#``
- optional octave digit
- optional ``.`` for dotted note
- optional leading duration (uses ``d`` if omitted)

Example
-------

..  code-block:: text

    test:d=4,o=5,b=140,k=am:8a,8b,8c6,8b,4a,4p,8e,8f,8e,2d

