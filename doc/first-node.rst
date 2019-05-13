==========
First Node
==========

For this section, we assume that you have successfully set-up IoTempower
on the Raspberry Pi and are able to access the local website on it.
Make sure that the IoT dongle is connected to the Raspberry Pi.

We will now set-up and configure our first IoT node (if you want to know
what a *node* is, check the `architecture chapter <architecture.rst>`_).

IoTempower supports among others the following microcontroller boards
(for more supported boards, check the `hardware chapter <hardware.rst>`_):

- Wemos D1 Mini Mini
- NodeMCU
- Espresso Lite V2

If you use a Wemos D1 Mini Mini as your first node (this is the default),
no change is necessary. If you want to use a NodeMCU, you will
have to change the config (in the file ``node.conf`` in your node folder)
to NodeMCU.

If you are in the classroom, your devices will be pre-flashed with the IoTempower
firmware on them.
If not please go to this page `First Flash page <pre-flash.rst>`_
on your Raspberry Pi to pre-flash your device.



Setting up the WiFi credentials on the node
-------------------------------------------

Note that this instructions are for reconfiguring the WiFi credentials on nodes
that have been pre-installed with IoTempower and can be flashed over wireless 
(over the air - OTA - the ones used in the classroom should be ready).
If your node has never been flashed with IoTempower before,
follow this tutorial here `First Flash page <pre-flash.rst>`_
for the first preparation.

Also note that the board does not have a display so we will use the blue LED
(the onboard light) to indicate the different configuration states.
(If you are using the Wemos D1 Mini and have a respective display shield,
you can plug it in and see some messages during the configuration process.)
Pay attention to LED during the whole process, whether it is on or off
and how it is blinking because this will indicate if the steps are
successfully working or not.

..   TODO: Insert image of the button shield!

-   Connect a button usually to D3 (or pin 0 depending on the board) against
    ground or a button shield (see image) on top of the Wemos D1 Mini.
    This will be used to set the node into *adoption mode*.

-   Power up your microcontroller via battery or usb power supply
    (connect it with a USB cable to either of these).
    Wait until the onboard_led starts blinking and press
    the button at least 2 times during these first blinking 5 seconds.
    If the led is continuously on (or continuously off),
    restart your board by powering
    it off and back on and try the process again.

-   If you have done the previous step correctly, the node is
    in adoption (or reconfiguration)
    mode and the onboard_led should immediately start blinking in a unique pattern:
    The onboard led will blink in some kind of Morse code: several long
    blinks and several short blinks, for example 1 long and 2 short blinks.
    Count the blinks in the pattern as it will be used to identify your node's
    in the next step. (If you had the Wemos Display Shield connected,
    it shoudl show you the code also on the small screen.)
    

Adopting the Node in the IoTempower environment on the Raspberry Pi
----------------------------------------------------------------

- If you have not done already, connect now your computer (laptop or
  desktop pc) to the Raspberry Pi's WiFi network (as described in the
  `Pi Quickstart tutorial <quickstart-pi.rst>`_).

- Connect to IoTempower via a web browser.

  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local,
    or if both don't work at https://192.168.12.1).

    If not already done previously, accept the security exception for the
    locally generated security certificate.

  - If you are asked for a user, use ``iotempower``,
    if you are asked for a password
    use ``iotempire``.

.. TODO: provide image of home page!

  -  You should now see the home page for your local IoTempower installation.

- Click on the link `IoT system example configuration folder
  </cloudcmd/fs/home/iotempower/iot-test/>`_ to navigate to
  the folder where the templates are.

.. TODO: provide image of the Iot system example page!

- Then, navigate into the `node1
  </cloudcmd/fs/home/iotempower/iot-test/node1/>`_ folder.

.. TODO: provide image of the Folder node1!

  You should see the folder view of the node1 folder, containing
  ``README.rst``, ``node.conf``, and ``setup.cpp``.

- When inside the ``node1`` folder click on the user menu button located on
  the bottom right corner of your web screen.
  The button to press is
  depicted below.

  .. image:: /doc/images/user-menu-button.png

.. TODO: provide image of the button menu for initializing!

- You will now see a menu with several buttons as options.

- Click on the button called ``Adopt/Initialize``
  and verify again that you are in the
  ``node1`` folder, then agree to start the process via selecting the
  ``Yes, run initialize``
  button.

.. TODO: Create a troubleshooting file!

- Wait until the process is done and make sure it was successful.
  You should see this msg *deploy successfully done.* and *Done initializing.*
  If it says *Initializing not successful, check errors above.* please refer
  to `troubleshooting <troubleshooting.rst>`_.


Congratulations!! Your node is now connect to IoTempower and the onboard-led can
be controlled with Node-RED using the button on this page
`</nodered/ui/#/1>`_.

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `Second Node <second-node.rst>`_.
`Versão em português aqui <first-node-pt.rst>`_.
