mfrc522
=======

..  code-block:: cpp

    mfrc522(name);

Create a new mfrc522 rfid/nfc tag reader device connected via SPI.
This device reports the data read as well as uuid and pic (in their own topics).
Via the set topic a value can be written to the tag
(only supported on MiFare classic tags). 


**other names**: rfid


Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

This driver always creates a subtopic tare. If any payload is sent to this
topic, the tare process on the scale is executed.

Example
-------

..  code-block:: cpp

    mfrc522(reader);

This example will report data of a supported RFID/tag in ``node_topic/reader``
and uid in ``node_topic/reader/uid`` (works on most NFC tags) as well as picc
in ``node_topic/reader/uid`.

Wiring
------

.. code-block::

   Wemos
   D1 Mini/    mfrc522/
   NodeMCU  -  rfid-rc522 board

       3V3  -  3.3V
        D8  -  sda
        D7  -  MOSI
        D6  -  MISO
        D5  -  SCK
        D0  -  RST
         G  -  GND
