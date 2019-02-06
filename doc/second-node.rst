==========
Second Node
==========

For this section, we assume that you have successfully set-up UlnoIoT
on the Raspberry Pi and are able to access the websites on it as well as
have set-up your `first node <first-node.rst>`_ and can control its onboard
led via the node-RED web gui.

The goal of this tutorial is to show you how to adopt and initialize a second
node and then wire these to together via node-red.

Let's get started:

-   Navigate back to your `IoT system example configuration folder
    <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/>`_

    As you have seen, each node has its own configuration folder in UlnoIoT.

-   For a second node, we need therefore another node-folder. We can quickly
    create one, based on a node-template.

    For that, make sure, you are still in the example system configuration
    folder and activate again the user menu.

    In the user menu, select ``Create New Node Folder``, and after verifying
    the destination path (which should be ``iot-test/``), select
    ``Yes, run create_node_template``. Confirm a (hopefully) successful
    creation, and you wil lbe back in cloudcmd's web file panel.

    You should see there a new folder called new-node (next to the old folder
    ``node1``, the ``README.rst`` and ``system.conf``).

-   Select the new folder by mouse or keyboard, and press the rename button
    (or the F2 key) and rename the folder into ``node2``.

-   Navigate into the ``node2`` folder, you should see the files:
    ``README.rst``, ``key.txt``, ``node.conf``, and ``setup.cpp``

-   If you are not using a Wemos D1 Mini as new node, edit ``node.conf`` and
    enter the new board.

-   Select ``setup.cpp`` and edit it (with edit button or F4 key). 


-   Note that this instructions are for reconfiguring
    the WiFi credentials on nodes
    that have been pre-installed with UlnIoT and can be flashed over wireless
    (over the air - OTA - the ones used in the classroom should be ready).
    If your node has never been flashed with UlnoIoT before,
    follow this tutorial here `First Flash page <pre-flash.rst>`_
    for the first preparation.

-   TODO: Insert image of the button shield!

    Connect a button usually to D3 (or pin 0 depending on the board) against
    ground or a button shield (see image) on top of the Wemos D1 Mini.
    This will be used to set the node into *adoption mode*.

-   Power up your microcontroller via battery or usb power supply
    (connect it with a USB cable to either of these).
    Wait until the onboard_led starts blinking and press
    the button 2 or 3 times during these first blinking 5 seconds.
    If the led is continuously on (or continuously off), 
    restart your board by powering
    it off and back on and try the process again.

-   If you have done the previous step correctly, the node is
    in adoption (or reconfiguration)
    mode and the onboard_led should be now blinking in a unique pattern:
    The onboard led will blink in some kind of Morse code: several long
    blinks and several short blinks, for example 1 long and 2 short blinks.
    Count the blinks in the pattern as it will be used to identify your node's
    WiFi network in the next step.

-   Now, on your smart phone or tablet (a computer works too), go to the
    WiFi settings menu. The node will show up in the list of WiFi networks
    called something like ``ulnoiot-ab-mn``. ``ab`` are unique identifiers and
    ``mn`` is relative to the unique blinking pattern where ``m`` is the number
    of long blinks and ``n`` number of short blinks.
    For example ``uiot-node-ab-12`` means 1 long and 2 short blinks.

    There might be quite a lot of networks (in a class setting). make sure you
    find the one matching your pattern (if there are several matches,
    chose the one with the strongest signal)

-   Click or tap to connect to the Node's WiFi network.

-   If there is an *Internet may not be available* warning click *OK*.

-   TODO: Insert image of WiFi credentials on the smart phone!
    You should be automatically forwarded to a web page like the one on the
    image (similar to logging into an open WiFi in a cafe or an airport).
    If that does not happen automatically, open any web browser on your phone
    and type http://192.168.4.1 into the URL selection box and click on
    ``Configure WiFi``.

-   Now enter network name and password of your Raspberry Pi's WiFi
    network (the one you configured for your UlnoIoT gateway)
    using the ``WiFi Name`` and ``Password`` that was setup
    previously like described in the
    `Pi Quickstart tutorial <quickstart-pi.rst>`_ and click on *save*.

-   If you have done this correctly, the node will reboot and
    the led will stay on continuously.
    If it starts blinking the unique pattern again,
    it means you have not configured it correctly.
    Then you have to repeat the previous step.

-   Now the node is in adoption mode and ready to be initialized.
    Move on to next part.


Adopting the Node in the UlnoIoT environment on the Raspberry Pi
----------------------------------------------------------------

- If you have not done already, connect now your computer (laptop or
  desktop pc) to the Raspberry Pi's WiFi network (as described in the
  `Pi Quickstart tutorial <quickstart-pi.rst>`_).

- Connect to UlnoIoT via a web browser.

  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local,
    or if both don't work at https://192.168.12.1).

    If not already done previously, accept the security exception for the
    locally generated security certificate.

  - If you are asked for a user, use ``ulnoiot``,
    if you are asked for a password
    use ``iotempire``.

  - TODO: provide image of home page!
    You should now see the home page for your local UlnoIoT installation.

- Click on the link `IoT system example configuration folder
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/>`_ to navigate to
  the folder where the templates are.

- TODO: provide image of the Iot system example page!
  You should see a page like the one bellow.

- Navigate into the `node1
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/node1/>`_ folder.

- TODO: provide image of the Folder node1!
  You should be seen a page like the one bellow.

- TODO: provide image of the weird button!
  When inside the ``node1`` folder click on the user menu button located on
  the bottom right corner of your web screen.

- TODO: provide image of the button menu for initializing!
  You will now see a menu with several buttons as options.

- Click on the button called ``Adopt/Initialize``
  and verify again that you are in the
  ``node1`` folder, then agree to start the process via selecting the 
  ``Yes, run initialize``
  button.

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