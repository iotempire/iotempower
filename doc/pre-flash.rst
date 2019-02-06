Pre-Flash devices
=================

When we buy our microcontrollers, they usually do not come pre-installed with
UlnoIoT. We have to manually write an initial firmware to our microcontrollers
at least once. Later, we can use the adoption method to re-configure
UlnoIoT nodes. We call this initialization process pre-flash.

If you want to pre-flash a Wemos D1 Mini, this process is very easy. Connect
the device, you want to pre-flash with a USB cable to the Pi gateway (or the
PC that runs ulnoiot) and call the user menu (or on PC ``ulnoiot shell``).
The user menu contains in the advance section a ``Pre-flash Wemos D1 Mini``
option. Just execute this option.

For other microcontrollers or in case something goes wrong with the adoption
process, navigate to a node folder that has the different microcontroller
configuration and call ``initialize serial`` (either from
the advanced user menu or manually from the command line).

Top: `ToC <index-doc.rst>`_, Previous: `First IoT Node <first-node.rst>`_,
Next: `Tool Support <tool-support.rst>`_.