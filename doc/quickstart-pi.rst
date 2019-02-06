Pi Quickstart
=============
After `installing the UlnoIoT image onto the SD-card <image-pi.rst>`_, plug it
back into an SD-card reader on any computer (MacOS, Windows or Linux) to
configure its WiFi router settings.

Setting up the WiFi-Router on the Pi
------------------------------------

- Open the SD-card.

- Find the ``wifi-setup.txt`` file
  (Windows might not show the ``.txt`` extension)

- Edit this ``wifi-setup.txt`` file. Change the default values under
  ``Wifi Name`` (#iotempire-123456) and ``Password`` (iotempire) to your own
  values. Make sure that ``Password`` is longer than 8 characters.

  .. code-block:: bash

     ### WiFi Name (avoid blanks) ###
     #iotempire-123456
     ### Password (at least 8 characters, max 32, avoid blanks) ###
     iotempire

  This configures the pi as a WiFi-router.
  Remember the ``WiFi Name`` and ``Password`` so you can log in to
  the WiFi network of the Pi from your computer.

- If you have access to ethernet (for example a free ethernet lan port on your
  router), connect the Pi to this ethernet port so that your Pi can access the
  internet - no extra configuration is necessary for this.
  If you do not have access to an ethernet port just move to the next step.
  [#f1]_

- Put the sd-card into a Raspberry Pi 3 and power it up.

- You should now see your ulnoiot WiFi network as specified in ``WiFi Name``.
  Connect your computer (laptop or desktop pc) to this WiFi network
  (use the password set as before for ``Password``).

Accessing the Local Services on the Raspberry Pi
------------------------------------------------

- You can now access UlnoIoT via a web browser.

- Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local or
  sometimes https://192.168.12.1). Accept
  the security exception for the locally generated security certificate. You
  should now see a link to the filesystem through cloud commander
  and an IoT system example configuration folder on the pi,
  as well as links to the Node-RED
  installation.

- If you are asked for a user, use ``ulnoiot``, if you are asked for a password
  use ``iotempire``.

- Open the link to the `IoT system example configuration folder
  </cloudcmd/fs/home/ulnoiot/iot-test>`_, you should now see you filesystem
  inside the IoT-test folder, listing a ``node1``-folder, a ``README.rst`` and
  a ``system.conf`` file. For now, ignore a potential second file panel to
  the right, we will first just concentrate here on the active panel.

- Check out the content of README.rst using the view button or view in the
  context menu (and exiting the view again with Escape or the x in the upper
  right corner).

- Navigate into the ``node1``-folder and check (as before) the content of
  ``setup.cpp``. A tiny bit cryptic? Don't fear this just says, we want to
  configure a device with a button and make the onboard blue led accessible,
  leave the content view of ``setup.cpp`` again.

.. If you have trouble following this, make sure to checkout the tutorials on
   youtube. TODO: provide webpage with links!

You can now continue with `First IoT Node <first-node.rst>`_.

.. rubric:: Footnotes

.. [#f1] If the Pi is not connected to the internet, a computer logged into its
         WiFi network might change the network back to another WiFi connecting
         as it prefers WiFi networks that are connected to the internet.
         Make sure to check that you are connected to the Pi-network on a
         regular basis.

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
`Versão em português aqui <quickstart-pi-pt.rst>`_.
