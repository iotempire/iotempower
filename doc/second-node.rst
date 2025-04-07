===========
Second Node
===========

For this section, we assume that you have either
successfully set-up IoTempower on the Raspberry Pi and are able
to access the websites on it or setup IoTempower in your
own Linux environment with access to a running mqtt broker and
a running Node-RED. On stand-alone systems, not the raspberry pi image, 
this is usually achieved by running ``iot x web_starter`` and then accessing
http://gateway-ip:40080 or http://localhost:40080. You can also tunnel port
40080 to your local computer with ``ssh iot@gateway-ip -L 40080:localhost:40080``
assuming that your iot user on yout gateway is ``iot``.

We also assume you have completed and running a node and
system folder from `first node <first-node.rst>`_ 
and can control its onboard led via the Node-RED web GUI.

The goal of this tutorial is to show you how to flash (or adopt)
a second node and then connect them together 
*over the air (OTA)* via Node-RED.

Let's get started...

New Node
--------

-   If you use the raspberry pi web GUI cloudcmd,
    navigate back to your `IoT system example configuration folder
    <https://iotgateway.local/cloudcmd/fs/home/iot/iot-test/>`_.
    Else navigate there by command line (for example ``cd ~/iot-systems/demo``).

    As you have seen, each node has its own configuration folder in IoTempower.

-   For a second node, we need therefore another node-folder. We can quickly
    create one, based on a node-template.

    For that, make sure you are still in the system configuration
    folder and activate again the user menu (or type ``iot menu``).

    In the user menu, select ``Create New Node Folder``, and after verifying
    the destination path (which should be ``iot-test/`` or ``iot-systems/demo``),
    select ``Yes, run create_node_template``. Confirm a (hopefully) successful
    creation and you will be back in cloudcmd's web file panel or the command
    line. You can also run ``create_node_template`` directly.

    You should see there a new folder called new-node (next to the old folder
    ``node1`` or ``test01``, the ``README.rst`` and ``system.conf``
    and eventually a sceraios folder).

-   In cloudcmd, select the new folder by mouse or keyboard, and press the rename button
    (or the F2 key) and rename the folder into ``node2`` - or rename it with the file
    manager directlyor with ``mv new-node my-node2-name``  on the command line.

-   Navigate into the ``node2`` or ``my-node2-name`` (for simplification, from now on 
    we refer to your second node folder with the name ``node2`` - change accordingly) folder,
    you should see the files:
    ``README.rst``, ``key.txt``, ``node.conf``, and ``setup.cpp``

-   If you are not using a Wemos D1 Mini as new node, edit ``node.conf`` and
    enter the respective new board.

-   Make sure to open another `IoTempower documentation web-page </>`_ for
    reference. And open from there the
    `command reference </doc/node_help/commands.rst>`_.
    (If you don't have the documentation server running,
    start it in a new terminal with ``iot doc serve`` and
    access it at http://localhost:8001 - not needed when using
    ``web_starter``).
    

-   Edit ``setup.cpp``
    Either with cloudcmd edit it (with edit button or F4 key)
    or in the command line with for example nano, micro, or tilde.

    You should see a small explanation comment block. You should now
    completely remove it to have an empty configuration file.

-   In the command reference, open the button help and copy from there the
    example configuration (including semicolon),
    something like this: ``input(lower, D2, "depressed", "pressed");``

    Paste this into the setup.cpp editor.

-   Change it to ``input(b1, D3, "up", "down");``

-   Leave the editor (press ok or by mouse the x in the upper right corner)
    and agree to save the file.

-   Like in the `first node tutorial <first-node.rst>`_ described, 
    adopt or flash (with deploy serial - eventually specifying the correct tty) 
    now node2. (for adoption: attach button, activate adoption mode, count Morse blinks, 
    use your phone to set own WiFi network credentials, finish calling
    adopt/initialize from user menu in ``node2`` folder).


Visually Programming the Connections
------------------------------------

-   If you don't use the Raspberry Pi image, you need to open now
    your Node-RED server page in the browser and build the whole
    integration flow analog to the description below.

-   If you use the pi image, you can head to this `Node-RED </nodered/>`_ page
    (only works in the raspberry pi image setup - else this will lead to an undefined page).
    Here you have to enter username (*admin*) and password (*iotempire*) again.

    You should see a Node-RED flow with five nodes on the raspberry pi.
    On a new installation, you will see an empty flow.

-   Notable here are the two pink nodes at the sides, one labeled ``node1/b1``
    and one ``node1/blue/set`` (if you don't use the pi image, you have to create this
    yourself).

    These are so-called mqtt (Message Queuing Telemetry Transport) nodes.
    They are our bridge to the IoT communication language.

-   The new node we just configured is sending data on the topic
    ``node2/b1``

-   Therefore, create a new mqtt input node by dragging it from the left into
    the current flow. Double click it and set its topic to ``node2/b1``.

-   Also create a debug output node (drag it into the pane) and draw a wire
    from the mqtt-node port to the debug port.

-   Press the red deploy button (in the upper right) corner.

-   Under the red deploy button is a little column with the symbol of a
    little bug, activate it and press the button connected to your new node
    several times.

    You should see several up and down messages in the debug log.

-   Add a ``change``-node and configure it so that it replaces `up` with
    `off` and `down` with `on`.

-   Wire this ``change``-node between the new mqtt input node for
    our ``node2`` button controller and the mqtt output labeled
    ``node1/blue/set``.

-   Deploy and press the button on ``node2``, observe the onboard led on
    ``node1``.

Congratulations!! You can remote control your led!

If you still have some time, try using this new button to toggle the led.
Study for this the filter/rbe and toggle node. 
(If you don't have the toggle node in Node-RED, you can get it here: 
https://flows.nodered.org/flow/020546215faf4bb2ca4b1ebcac609154.)
What is happening in them?


Top: `ToC <index-doc.rst>`_, Previous: `First Node <first-node.rst>`_,
Next: `External Resources <resources.rst>`_.
`Versão em português aqui <second-node-pt.rst>`_.
