Pi Quickstart
=============
After `installing the IoTempower image onto the SD-card <image-pi.rst>`_, plug it
back into an SD-card reader on any computer (MacOS, Windows or Linux) to
configure its WiFi router settings.

Setting up the WiFi-Router on the Pi
------------------------------------

- Open the SD-card.

- Find the ``wifi.txt`` file
  (Windows might not show the ``.txt`` extension)

- Edit this ``wifi.txt`` file. Change the default values under
  ``Wifi Name`` (#iotempire-123456) and ``Password`` (iotempire) to your own
  values. Make sure that ``Password`` is longer than 8 characters.
  Also make sure to remove the pound sign (#) from the ``Wifi Name`` line - else it is ignored.

  ..  code-block:: bash

      ### WiFi Name (avoid blanks), when commented out, pick iotempower+mac-id ###
      #iotempire-123456
      ### Password (at least 8 characters, max 32, avoid blanks) ###
      iotempire

  After your changes, it might for example look like this:
  
  ..  code-block:: bash

      ### WiFi Name (avoid blanks), when commented out, pick iotempower+mac-id ###
      my-iot-network
      ### Password (at least 8 characters, max 32, avoid blanks) ###
      secretpassword

  This configures the pi as a WiFi-router.
  Remember the ``WiFi Name`` and ``Password`` so you can log into
  the WiFi network of the Pi from your computer.
  (You can skip the password if you want to conenct to an open network.)

- If you have access to ethernet (for example a free ethernet lan port/socket
  on your
  router), connect the Pi to this ethernet port so that your Pi can access the
  internet - no extra configuration is necessary for this.
  You can also connect an Android-phone with its charging cable to one of
  the USB ports of the pi and enable USB-tethering (if the Android phone) was
  connected to wifi before it will forward its ethernet to the pi gateway.
  If you do not have access to an ethernet port or an Android phone
  just move to the next step.
  [#f1]_

- Put the sd-card into a Raspberry Pi 4 and power it up.

- You should now see your IoTempower WiFi network as specified in ``WiFi Name``.
  Connect your computer (laptop or desktop pc) to this WiFi network
  (use the password set as before for ``Password``).


Accessing the Local Services on the Raspberry Pi
------------------------------------------------

- You can now access IoTempower via a web browser.

- Point your browser at https://gw.iotempire.net
  Yes, this global url should work and when connected to the internet
  also not complain about any certificate issues.
  If you have trouble connecting with this global URL, you can also try
  https://192.168.12.1, https://iotgateway,
  or https://iotgateway.local. One of them should open the homepage of
  your IoTempower pi gateway installation.
  (If you don't have internet on the gateway and use one of the latter options, accept
  the security exception for the locally generated security certificate.)
  
  You should now see the IoTempower homepage with links to Node-RED
  and the local documentation.

- If you are asked for a user, use ``iot`` (for Node-RED and portainer you want to use admin),
  if you are asked for a password use ``iotempire``.

- Keep this home-page for later (remember or bookmark).

- Now open in another tab this documentation locally on the gateway:

  This should be 
  https://gw.iotempire.net/doc/quickstart-pi.rst
  (or https://192.168.12.1/doc/quickstart-pi.rst,
  https://iotgateway/doc/quickstart-pi.rst,
  or https://iotgateway.local/doc/quickstart-pi.rst).

  Continue to read the documentation from there.

- Connect to the gateway via SSH to explore the example IoT configuration:

  .. code-block:: bash

     ssh iot@iotgateway

  (Use ``iot@192.168.12.1`` if hostname resolution does not work.
  The default password is ``iotempire``.)

- Once logged in, list the IoT test folder:

  .. code-block:: bash

     ls ~/iot-test

  You should see a ``node1`` folder, a ``README.rst`` and a ``system.conf``
  file.

- Check out the content of ``README.rst``:

  .. code-block:: bash

     cat ~/iot-test/README.rst

- Navigate into the ``node1`` folder and check the content of ``setup.cpp``:

  .. code-block:: bash

     cat ~/iot-test/node1/setup.cpp

  A tiny bit cryptic? Don't fear! This just says we want to configure a
  device with a button and make the onboard blue LED accessible.

.. If you have trouble following this, make sure to checkout the tutorials on
   youtube. TODO: provide webpage with links!

You can now continue with `First IoT Node <first-node.rst>`_.

.. rubric:: Footnotes

.. [#f1] If the Pi is not connected to the internet, a computer logged into its
         WiFi network might change the network back to another WiFi connection
         as it prefers WiFi networks that are connected to the internet.
         Make sure to check that you are connected to the Pi-network on a
         regular basis.

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
