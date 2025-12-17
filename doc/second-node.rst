======================
Second Node (Advanced)
======================

.. note::
   **New to IoTempower?** The `Quickstart Guide <quickstart.rst>`_ includes 
   instructions for adding a second node in the "Your Second Node" section.
   
   This advanced tutorial provides more detailed explanations for users who 
   want to understand the system more deeply.

For this section, we assume that you have IoTempower set up on your system
with access to a running mqtt broker and Node-RED. This is usually achieved 
by running ``iot x web_starter`` and then accessing http://localhost:40080 
or http://gateway-ip:40080 (you can tunnel port 40080 to your local computer 
with ``ssh user@gateway-ip -L 40080:localhost:40080``).

We also assume you have completed and running a node and
system folder from the `first node tutorial <first-node.rst>`_ 
(or from the quickstart manual) and can control its onboard led via the Node-RED web GUI.

The goal of this tutorial is to show you how to flash (or adopt)
a second node and then connect them together 
*over the air (OTA)* via Node-RED.

Let's get started...

New Node
--------

-   Navigate to your IoT system configuration folder.
    For example, ``cd ~/iot-systems/demo`` (or whatever you named your system).

    As you have seen, each node has its own configuration folder in IoTempower.

-   For a second node, we need another node-folder. We can quickly
    create one, based on a node-template.

    Make sure you are in the system configuration folder and use the 
    user menu: ``iot menu``

    In the user menu, select ``Create New Node Folder``, and after verifying
    the destination path (which should be your system folder like ``iot-systems/demo``),
    select ``Yes, run create_node_template``. Confirm a (hopefully) successful
    creation.
    
    You can also run ``create_node_template`` directly from the command line.

    You should see a new folder called ``new-node`` (next to your first node 
    folder like ``node1`` or ``test01``, the ``README.rst`` and ``system.conf``
    and possibly a scenarios folder).

-   Rename the folder to ``node2``:

    .. code-block:: bash

       mv new-node node2

-   Navigate into the ``node2`` folder:

    .. code-block:: bash

       cd node2
    
    You should see the files:
    ``README.rst``, ``key.txt``, ``node.conf``, and ``setup.cpp``

-   If you are not using a Wemos D1 Mini as new node, edit ``node.conf`` and
    enter the respective new board.

-   For reference documentation on available commands and device types,
    see the `Command Reference <node_help/commands.rst>`_ or use 
    ``iot x web_starter`` which includes the documentation server.
    
-   Edit ``setup.cpp`` using your preferred editor (nano, micro, etc.):

    You should see a small explanation comment block. You should now
    completely remove it to have an empty configuration file.

-   In the command reference, look up the button configuration and copy an
    example like: ``input(lower, D2, "released", "pressed");``

    Paste this into the setup.cpp editor.

-   Change it to ``input(b1, D3, "up", "down");``

-   Save the file (Ctrl+S in micro, Ctrl+O then Enter in nano).

-   As described in the `first node tutorial <first-node.rst>`_, 
    flash node2 using ``deploy serial`` (or adopt it if you're using that method).
    
    If you need to specify a different serial port, use:
    
    .. code-block:: bash

       deploy serial /dev/ttyUSB1  # or use the shortcut: deploy serial usb1


Visually Programming the Connections
------------------------------------

-   Open your Node-RED server page in the browser at http://localhost:40080/nodered.

    .. note::
       **Optional: Using the Raspberry Pi Image**
       
       If you're using the IoTempower Raspberry Pi image, you may need to enter 
       username (*admin*) and password (*iotempire*) to access Node-RED. You 
       will see a pre-configured flow with example nodes.
       
       On a fresh installation, you will start with an empty flow.

-   You should see the Node-RED flow editor. Notable elements include the pink 
    mqtt nodes on the sides - these are your bridge to IoT communication.

-   The new node we just configured is sending data on the topic ``node2/b1``

-   Create a new mqtt input node by dragging it from the left palette into
    the flow. Double click it and set its topic to ``node2/b1``.

-   Also create a debug output node (drag it from the palette into the flow) and draw a wire
    from the mqtt-node output port to the debug node input port.

-   Press the red deploy button (in the upper right corner).

-   Under the red deploy button is a column with icons. Click the bug icon 
    to open the debug sidebar. Now press the button connected to your new node
    several times.

    You should see several "up" and "down" messages in the debug log.

-   Add a ``change``-node (from the function section in the palette) and 
    configure it to replace ``up`` with ``off`` and ``down`` with ``on``.

-   Wire this ``change``-node between the mqtt input node for
    ``node2/b1`` and the mqtt output node labeled
    ``node1/blue/set`` (you may need to create this output node if it doesn't exist).

-   Deploy and press the button on ``node2``. You should see the onboard led on
    ``node1`` turn on and off!

Congratulations!! You can remote control your led!

If you still have some time, try using this new button to toggle the led.
Study the filter/rbe and toggle nodes for this. 
(If you don't have the toggle node in Node-RED, you can get it here: 
https://flows.nodered.org/flow/020546215faf4bb2ca4b1ebcac609154.)
What is happening in them?


Top: `ToC <index-doc.rst>`_, Previous: `First Node <first-node.rst>`_,
Next: `External Resources <resources.rst>`_.
`Versão em português aqui <second-node-pt.rst>`_.
