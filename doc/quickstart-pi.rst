Pi Quickstart
=============
After `installing the UlnoIoT image on the SD-card <image-pi.rst>`_, replug it 
into the SD-card reader on any computer (MacOS, Windows or Linux).

Setting up the wifi on the Pi
-----------------------------

- Open the SD-card.

- Find the ``wifi-setup.txt`` file (Windows might not show the .txt extension)

- Edit this ``wifi-setup.txt`` file. Change the default values under 
  ``Wifi Name`` (#iotempire-123456) and ``Password`` (iotempire) to your own 
  values. Make sure that ``Password`` is longer than 8 characters. 

  .. code-block:: bash
    
     ### Wifi Name (avoid blanks) ###
     #iotempire-123456
     ### Password (at least 8 characters, max 32, avoid blanks) ###
     iotempire

  This configures the pi as a wifi-router. 
  Remember the ``Wifi Name`` and ``Password`` so you can log in to
  the Wifi network of the Pi from your computer. 

- If you have access to ethernet (for example a free ethernet lan port on your 
  router), connect the Pi to this ethernet port so that your Pi can access the 
  internet - no extra configuration is necessary for this. 
  If you do not have access to an ethernet port just move to the next step. [#f1]_

- Put the sd-card into a Raspberry Pi 3 and power it up.

- You should now see your ulnoiot wifi network as specified in ``Wifi Name``.
  Connect your computer (laptop or desktop pc) to this wifi network
  (use the password set as before for ``Password``). 

- You can now connect to your Pi via a web browser.
  
  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local). Accept
    the security exception for the locally generated security certificate. You
    should now see a link to the filesystem through cloud commander
    and an IoT testsystem on the pi,
    as well as links to the Node-RED installation.
    
    TODO: change! Cloudcmd allows you to open a small console through typing '
    
  - If you are asked for a user, use ``ulnoiot``, if you are asked for a password
    use ``iotempire``.

- TODO: adapt! Run in ssh or the console (type and hit enter) the command ``ulnoiot upgrade`` to make sure that
  you have the latest version of ulnoiot.


If you have trouble following this, make sure to checkout the tutorials on
youtube. TODO: provide webpage with links!

You can now continue with `First IoT Node <first-node.rst>`_.

This quickstart is also available in Portuguese `<quickstart-pi-pt.rst>`_.

.. rubric:: Footnotes

.. [#f1] If the Pi is not connected to the internet a computer logged in to its
         Wifi network might change network back to another wifi connecting as 
         it preferes wifi networks that are connected to the internet.