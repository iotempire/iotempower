m5stickc_mic
============

**Available on M5StickC Plus and M5StickC Plus2**

..  code-block:: cpp

    m5stickc_mic(name);

**other names:** ``m5stickc_microphone``

Captures audio from the built-in PDM microphone on M5StickC Plus and M5StickC
Plus2 devices, and publishes raw 16-bit PCM audio buffers over MQTT.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

MQTT Interface
--------------

The microphone device creates the following MQTT topics:

- ``<node>/name/audio`` → Binary payload: raw 16-bit signed PCM samples at
  16 kHz mono, 1024 samples (2048 bytes) per publish.

The audio topic publishes binary data whenever a complete buffer of 1024 samples
has been recorded. Only one buffer is queued at a time; if the previous buffer
has not yet been published, the new recording is skipped to avoid back-pressure.

Recording Details
-----------------

.. list-table::
   :header-rows: 1

   * - Parameter
     - Value
   * - Sample rate
     - 16 000 Hz
   * - Bit depth
     - 16-bit signed integer (int16_t)
   * - Channels
     - Mono
   * - Buffer size
     - 1024 samples (64 ms audio)
   * - Encoding
     - Raw PCM (little-endian)

Example
-------

**node name:** ``living_room/stick1``

..  code-block:: cpp

    m5stickc_mic(mic);

Subscribe to the audio stream:

..  code-block:: bash

    mosquitto_sub -t "living_room/stick1/mic/audio" | \
        aplay -f S16_LE -r 16000 -c 1

Decode in Python using NumPy:

..  code-block:: python

    import numpy as np

    def on_audio(client, userdata, msg):
        samples = np.frombuffer(msg.payload, dtype=np.int16)
        # samples is a 1-D array of 1024 int16 values at 16 kHz

Notes
-----

- The microphone shares the I2S peripheral with the built-in speaker.
  The device automatically disables the speaker (``StickCP2.Speaker.end()``) during
  initialization so that the microphone can use I2S.
- On M5StickC Plus, ``M5Unified`` is used; on M5StickC Plus2, ``M5StickCPlus2``
  is used.  The device source is shared between the two boards via the
  ``m5stickc_plus`` node-type inheritance.
