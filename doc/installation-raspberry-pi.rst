Please also check out the tutorial videos for this setup on ulno's youtube
channel(s): https://www.youtube.com/@ut-teaching-ulno/search?query=gateway


Installation on Raspberry Pi from Pre-Prepared Image
++++++++++++++++++++++++++++++++++++++++++++++++++++

Tutorial video for setting up the sd-card for the pi: https://youtu.be/FrIIXsseZys

1. Installation step by step:

- `Install the IoTempower image onto an SD-card <image-pi.rst>`_

- `Set up the credentials of the WiFi-Router on the Pi
  <quickstart-pi.rst#installation#setting-up-the-wifi-router-on-the-pi>`_

- If you have another USB-wifi stick and want to use the Internet via WiFi,
  connect this WiFi stick to the pi and make corresponding changes to the
  file ``/boot/wifi-in.txt``, setting ``WiFi Name/SSID``, ``WiFi Password``,
  and eventually the ``WiFi business network user``.

  Careful! Lots of WiFi sticks do not work correctly on the
  Raspberry Pi. Make sure you have verified that it works under the normal
  Raspberry Pi desktop environment.

  Much preferred is connecting the Pi to ethernet. It avoids all the
  aforementioned potential WiFi problems. If you have access to ethernet
  (for example, a free ethernet LAN port on your
  router), connect the Pi to this ethernet - no extra configuration is
  necessary for this.

- You can now connect to the pi gateway via a web browser or ssh

- https://iotgateway (or https://iotgateway.local or
  sometimes https://192.168.12.1). Accept
  the security exception for the locally generated security certificate. You
  should now see a link to the filesystem through cloud commander (cloudcmd)
  and an IoT testsystem on the pi,
  as well as links to the Node-RED installation.

- On the raspberry pi IoTempower installation, you can use cloudcmd to access
  and interact with your system.

- If you are asked for a user, use ``iot``, if you are asked for a password
  use ``iotempire``.

- ssh access (this is for advanced users, usually you can just use the
  browser):

  - For ssh access in Windows,
    install `Moba xterm <https://mobaxterm.mobatek.net/>`__ 
    or `Git and integrated ssh for Windows <https://git-scm.com/download/win>`__.

  - On MacOS,
    make sure, you have `iTerm2 <https://iterm2.com/>`__ and optionally
    `XQuartz <https://www.xquartz.org/>`__ installed.

  - Linux will work out of the box.

  - On Android use termux.

  - For Mac or Linux you can use the built in ssh commands in the terminal.
    Make sure to enable X forwarding to have
    the clipboard working transparently.

    The command for Mac and Linux is:

    ``ssh -X iot@iotgateway``

    The command on Windows will be (and can be graphically
    configured in MobaSSH):

    ``ssh -X iot@192.168.12.1``

    The default password for the user IoTempower is ``iotempire``

    Consider changing it immediately entering the command ``passwd``

  - At one point, also make sure to run ``sudo raspi-config`` and
    chose to resize the
    hd in the advanced options.

..  - Check out the `short tmux help </doc/tmux-help.txt>`__,
    pressing the ctrl-key and a-key simultaneously,
    releasing them and then pressing the h-key.

  - Run in ssh or the terminal (type and hit enter) the command
    ``iot upgrade`` to make sure that
    you have the latest version of iotempower.


You can now continue with `First IoT Nodes <first-node.rst>`_.



Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
