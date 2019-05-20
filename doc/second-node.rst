===========
Second Node
===========

For this section, we assume that you have successfully set-up UlnoIoT
on the Raspberry Pi and are able to access the websites on it as well as
have set-up your `first node <first-node.rst>`_ and can control its onboard
led via the node-RED web gui.

The goal of this tutorial is to show you how to adopt and initialize a second
node and then connect them together *over the air (OTA)* via Node-RED.

Let's get started...

New Node
--------

-   Navigate back to your `IoT system example configuration folder
    <https://iotgateway.local/cloudcmd/fs/home/iotempower/iot-test/>`_

    As you have seen, each node has its own configuration folder in UlnoIoT.

-   For a second node, we need therefore another node-folder. We can quickly
    create one, based on a node-template.

    For that, make sure, you are still in the example system configuration
    folder and activate again the user menu.

    In the user menu, select ``Create New Node Folder``, and after verifying
    the destination path (which should be ``iot-test/``), select
    ``Yes, run create_node_template``. Confirm a (hopefully) successful
    creation, and you wil be back in cloudcmd's web file panel.

    You should see there a new folder called new-node (next to the old folder
    ``node1``, the ``README.rst`` and ``system.conf``).

-   Select the new folder by mouse or keyboard, and press the rename button
    (or the F2 key) and rename the folder into ``node2``.

-   Navigate into the ``node2`` folder, you should see the files:
    ``README.rst``, ``key.txt``, ``node.conf``, and ``setup.cpp``

-   If you are not using a Wemos D1 Mini as new node, edit ``node.conf`` and
    enter the new board.

-   Make sure to open another `IoTempower documentation web-page </>`_ for
    reference. And open from there the
    `command reference </doc/node_help/commands.rst>`_.

-   In the file view, select ``setup.cpp``
    and edit it (with edit button or F4 key).

    You should see a small explanation comment block. You should now
    completely remove it to have an empty configuration file.

-   In the command reference open the button help and copy from there the
    example configuration (including semicolon),
    something like this: ``input(lower, D2, "depressed", "pressed");``

    Paste this into the setup.cpp editor.

-   Change it to ``input(b1, D3, "up", "down");``

-   Leave the editor (press ok or by mouse the x in the upper right corner)
    and agree to save the file.

-   Like in the `first node tutorial <first-node.rst>`_ described, adopt now
    node2. (attach button, activate adoption mode, count Morse blinks, 
    use phone to set own WiFi network credentials, finish calling
    adopt/initialize from user menu in ``node2`` folder).


Visually Programming the Connections
------------------------------------

-   After successful adoption, open the `Node-RED </nodered/>`_ page. Here you
    have to probably enter username (*iotempower*) and password (*iotempire*) again.

    You should see a Node-RED flow with five nodes.

-   Notable here are the two pink nodes at the sides, one labeled ``node1/b1``
    and one ``node1/blue/set``

    These are so called mqtt (Message Queuing Telemetry Transport) nodes.
    They are our bridge to the IoT communication language.

-   The new node, we just configured, is sending data on the topic
    ``node2/b1``

-   Therefore, create a new mqtt input node, by dragging it from the left into
    the current flow. Double click it and set its topic to ``node2/b1``.

-   Also create a debug output node (drag it into the pane) and draw a wire
    from the mqtt-node port to the debug-port.

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

-   Deploy and press the button on ``node2``, observer the onboard led on
    ``node1``.

Congratulations!! You can remote control your led!

If you still have some time, try to use this new button to actually toggle
the led. Study for this the rbe and toggle node. What is happening in them?


Top: `ToC <index-doc.rst>`_, Previous: `First Node <first-node.rst>`_,
Next: `External Resources <resources.rst>`_.
`Versão em português aqui <second-node-pt.rst>`_.