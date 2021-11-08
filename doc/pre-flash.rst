Pre-Flash devices
=================

When we buy our microcontrollers, they usually do not come pre-installed with
IoTempower. We have to manually write an initial firmware to our microcontrollers
at least once. Later, we can use direct serial flashing or the adoption method
to re-configure IoTempower nodes. We call this initialization process pre-flash.

If you want to pre-flash a Wemos D1 Mini, this process is very easy. Connect
the device, you want to pre-flash with a USB cable to the Pi gateway (or the
Linux PC that runs IoTempower) and call the user menu (or on PC ``iot menu``).
The user menu contains in the advance section a ``Pre-flash Wemos D1 Mini``
option. Just execute this option.

For other microcontrollers or in case something goes wrong with the adoption
process, navigate to a node folder that has the different microcontroller
configuration and call ``initialize serial`` (either from
the advanced user menu or manually from the command line).



   For Windows (WSL 1) Users:

   - Start IoTempower (open terminal and type ``iot`` + Enter key)

   - Make note of the COM port that the Device Manager Shows (for example (COM4))

   - Run the following command, but replace the y with the number after COM (in the above example y = 4)
	
     ``pre_flash_wemos ttySy``

     If you want to overwrite a Wemos flashed as dongle, you need to specify ``force``
     as last option.

   - For Linux Users the port is usually guessed correctly automatically

     ``pre_flash_wemos``

   If you are getting a /dev/ttySy permission denied error, add yourself to the dialout group or run the command with sudo.

   If you are getting that /dev/ttySy error related to Wemos not being plugged in,
   most likely you have incorrectly specified which ttySy port 
   Wemos is connected to.

   Wait for the pre_flash_wemos command to finish, it should take about 2-5 minutes.




Top: `ToC <index-doc.rst>`_, Previous: `First IoT Node <first-node.rst>`_,
Next: `Tool Support <tool-support.rst>`_.