IoTknit is part of the IoTempower framework.
It allows to easily knit together devices reachable via 
MQTT (and hopefully later also for classic webrequests and websockets)
of a network into an actual IoT system.
It is therefore an IoT integration system (similar but much more
barebones than `Node-RED <https//nodered.org>`_).
It was called initially IntegrIoT since 2017, but that name was also adopted
by `BM Communications <https://www.bmit.cz>`_ in 2022 and therefor
had to be changed to forego any potential legal conflicts
for this open source project here.

It is a Python module and can be installed to your local
Python environment the following way:

.. code-block:: bash

   pip install .

For building, run:

.. code-block:: bash

   pip -m build

Examples can be found at:
https://github.com/iotempire/iotempower/tree/master/examples/iotknit