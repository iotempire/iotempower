==========
First Node
==========

For this section, we assume that you have successfully set-up UlnoIoT
in the Raspberry Pi.
UlnoIoT supports the following microcontroller boards:

- Wemos D1 Mini Mini
- NodeMCU
- Espresso Lite V2
- Sonoff and Sonoff Touch
- There is an esp8266 generic option for other esp8266-based boards.

You will need a smart phone or a computer for this first node setup.

If you use a Wemos D1 Mini Mini as your first node (this is the default),
no change is necessary. If you want to use a NodeMCU, change the config
to NodeMCU.

If you are in the classroom your devices will be pre-flashed with the UlnoIoT
firmware on them.
If not please go to this page `First Flash page <pre-flash.rst>`_
on your Raspberry Pi to pre-flash your device.



Setting up the WiFi credentials on the node
-------------------------------------------

-   TODO: Adapt and link a tutorial for serial initialization!

    Note that this instructions are for reconfiguring
    the WiFi credentials on nodes
    that have been pre-installed with UlnIoT and can be flashed over wireless
    (over the air - OTA - the ones used in the classroom should be ready).
    If your node has never been flashed with UlnoIoT before,
    follow this tutorial here `First Flash page <pre-flash.rst>`_
    for the first preparation.

-   TODO: Insert image of the button shield!

    Connect a button usually to D3 (or pin 0 depending on the board) against
    ground or a button shield (see image) on top of the Wemos D1 Mini.
    This will be used to set the node in adopting-mode.

-   Power up your microcontroller via battery or usb power supply.
    Wait until the onboard_led starts blinking and press
    the button 2 or 3 times during this first blinking 5 seconds.
    While that is happening press the button 3 times. If the led is
    continuously on (or continuously off), restart your board by powering
    it off and back on and try the process again.

-   If you have done previous step correctly, the node is in reconfiguration
    mode and the onboard_led should be now blinking in a unique pattern
    (The onboard led will blink in some kind of Morse code several long
    blinks and several short blinks, for example 1 long and 2 short blinks.
    Count the blinks in the pattern as it will be used to identify your node's
    WiFi network in the next step.

-   Now on your smart phone or tablet (a computer works too) go to the
    WiFi settings menu. The node will show up in the list of WiFi networks
    called something like ``ulnoiot-ab-mn``. ``ab`` are unique identifiers and
    ``mn`` is relative to the unique blinking pattern where ``m`` is the number
    of long blinks and ``n`` number of short blinks.
    For example ``uiot-node-ab-12`` means 1 long and 2 short blinks.

-   Click or tap to connect to the Node's WiFi network.

-   If there is an *Internet may not be available* warning click *OK*.

-   TODO: Insert image of WiFi credentials on the smart phone!
    You should be automatically prompt to a web page like the one on the image.
    If that does not happen automatically, open any web browser on your phone
    and type http://192.168.4.1 in the URL selection box.

-   Click on *Configure WiFi* and connect to the Raspberry Pi's WiFi
    network using the ``WiFi Name`` and ``Password`` that was setup
    previously on `<quickstart-pi.rst>`_ and click on *save*.

-   If you have done it correctly, the node will reboot and
    the led will stay on continuously.
    If it starts blinking the unique pattern again
    it means you have not configured it correctly
    so repeat the previous step.

-   Now the node is in adoption mode and ready to be initialized.
    Move on to next part.


Adopting the Node in the UlnoIoT environment on the Raspberry Pi
----------------------------------------------------------------

- If you have not done already, connect now your computer (laptop or
  desktop pc) to the Raspberry Pi's WiFi network.

- You can now connect to UlnoIoT via a web browser.

  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local).
    Accept the security exception for the locally generated security
    certificate.

  - If you are asked for a user, use ``ulnoiot``,
    if you are asked for a password
    use ``iotempire``.

  - TODO: provide image of home page!
    You should now see the home page for UlnoIoT.

- Click on the link `Iot system example
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/>`_ to navigate to
  the folder where the templates are.

- TODO: provide image of the Iot system example page!
  You should be seen a page like the one bellow.

- Navigate to the `Node 1
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/node1/>`_ folder.

- TODO: provide image of the Folder 1!
  You should be seen a page like the one bellow.

- TODO: provide image of the weird button!
  When inside the *folder 1* click on the console button located on
  the bottom right corner your screen.

- TODO: provide image of the button menu for initializing!
  You will now see a menu link this one on the image bellow.

- Click on the button initialize.

- TODO: Create a troubleshooting file!
  Wait until the process is done and make sure it was successful.
  You should see this msg *deploy successfully done.* and *Done initializing.*
  If it says *Initializing not successful, check errors above.* please refer
  to `troubleshooting <troubleshooting.rst>`_.


Congratulations!! Your node is now connect to UlnIoT and the onboard-led can
be controlled with Node-RED using the button on this page
`<https://ulnoiotgw/nodered/ui/#/1>`_

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `External Resources <resources.rst>`_.