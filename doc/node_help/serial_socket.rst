serial_socket
=============

..  code-block:: cpp

    serial_socket(name, rx_pin, tx_pin /*, baud is by default 9600*/ /*, config is by default SWSERIAL_8N1*/ /*, invert is by default false*/);

Create a new software serial connector socket device.

The socket publishes the received serial data on the device topic and sends
payloads received on the ``/set`` topic to the serial TX pin.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network.
  Inside the code it can be addressed via IN(name).

- ``rx_pin``: GPIO pin for the software serial RX line.

- ``tx_pin``: GPIO pin for the software serial TX line.

- ``baud``: serial baud rate (default 9600).

- ``config``: serial data configuration (default ``SWSERIAL_8N1``).

- ``invert``: set true to invert serial levels (default false).

Example
-------

node name: ``lab1/experiment2``:

..  code-block:: cpp

    serial_socket(uart1, D5, D6, 38400);

This creates a software serial socket device that listens on D5/D6 and publishes
received data under ``lab1/experiment2/uart1``. Sending data to
``lab1/experiment2/uart1/set`` writes the payload to the serial TX pin.
