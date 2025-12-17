=======================
Serial to Network Setup
=======================

This guide explains how to set up ser2net on an OpenWRT router to enable 
remote serial flashing and monitoring of IoT devices over the network.

Overview
========

ser2net allows you to access serial devices connected to your router's USB 
ports over the network using the RFC2217 protocol. This is particularly 
useful for:

- Flashing IoT nodes without physically connecting them to your computer
- Remote serial console access for debugging
- Managing multiple devices through a central router

Prerequisites
=============

- An OpenWRT router with USB ports (e.g., GL.iNet Mango)
- SSH access to your router
- Basic familiarity with the OpenWRT web interface or command line

Installation
============

1. Log into your router's web interface or SSH into it

2. Install the required packages:

   Via Web Interface:
   
   - Go to System → Software
   - Update package lists
   - Search for and install:
     
     - ``ser2net``
     - ``luci-app-ser2net`` (optional, provides web UI)
     - ``kmod-usb-serial-ftdi`` (for FTDI-based USB serial adapters)
     - ``kmod-usb-serial-cp210x`` (for CP210x-based adapters)
     - ``kmod-usb-serial-ch341`` (for CH341-based adapters)
     - ``kmod-usb-serial-pl2303`` (for PL2303-based adapters)

   Via SSH:
   
   .. code-block:: bash

      opkg update
      opkg install ser2net luci-app-ser2net
      opkg install kmod-usb-serial-ftdi kmod-usb-serial-cp210x \
                   kmod-usb-serial-ch341 kmod-usb-serial-pl2303

Configuration
=============

Basic ser2net Configuration
---------------------------

1. Connect your microcontroller to the router's USB port

2. Check that the device is detected:

   .. code-block:: bash

      ls /dev/ttyUSB*
      # Should show /dev/ttyUSB0 or similar

3. Edit the ser2net configuration file:

   .. code-block:: bash

      vi /etc/ser2net.conf

4. Add a configuration line for your device. For example, to expose 
   ``/dev/ttyUSB0`` on port 5000 with RFC2217 support:

   .. code-block:: text

      5000:telnet:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT banner

   For RFC2217 with flow control (recommended for flashing):

   .. code-block:: text

      5000:raw:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT XONXOFF LOCAL remctl

5. Restart ser2net:

   .. code-block:: bash

      /etc/init.d/ser2net restart
      /etc/init.d/ser2net enable  # Enable on boot

Using luci-app-ser2net (Web Interface)
--------------------------------------

If you installed ``luci-app-ser2net``:

1. Go to Services → ser2net in the web interface

2. Enable the service

3. Add a new proxy configuration:
   
   - **Port**: 5000
   - **Device**: /dev/ttyUSB0
   - **Baud Rate**: 115200
   - **Protocol**: RFC2217
   - **Options**: Enable RTS/CTS flow control

4. Save and Apply

Using with IoTempower
=====================

Flashing Devices
---------------

Once ser2net is configured, you can flash devices remotely using:

.. code-block:: bash

   deploy serial rfc2217://192.168.8.1:5000

Replace ``192.168.8.1`` with your router's IP address and ``5000`` with 
your configured port.

Serial Console Access
--------------------

To access the serial console of a connected device:

.. code-block:: bash

   console_serial rfc2217://192.168.8.1:5000

Force Flash Mode
---------------

Some microcontrollers (like the Wemos D1 Mini) need to be put in flash mode 
manually:

- For Wemos D1 Mini: Connect GPIO D3 to GND before powering on
- For NodeMCU: Hold the FLASH button while pressing RESET

After flashing is complete, remove the connection and reset the device.

Troubleshooting
===============

Device Not Detected
------------------

If ``/dev/ttyUSB*`` doesn't appear:

1. Check USB connection is secure
2. Verify the correct kernel module is installed for your adapter
3. Check kernel messages: ``dmesg | tail``
4. Try a different USB port

Connection Refused
-----------------

If you get "Connection refused" errors:

1. Verify ser2net is running: ``/etc/init.d/ser2net status``
2. Check the configuration file for syntax errors
3. Ensure the port is not blocked by the firewall
4. Test locally on the router: ``telnet localhost 5000``

Flash Failures
-------------

If flashing fails:

1. Ensure the device is in flash mode (see Force Flash Mode above)
2. Check baud rate matches (usually 115200)
3. Enable flow control in ser2net configuration
4. Try a shorter USB cable
5. Check power supply is adequate

Permission Denied
----------------

If you get permission errors:

1. Check device permissions: ``ls -l /dev/ttyUSB0``
2. Add ser2net user to dialout group: ``usermod -a -G dialout ser2net``
3. Restart ser2net

Multiple Devices
===============

To support multiple devices, add multiple port configurations:

.. code-block:: text

   5000:raw:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT XONXOFF LOCAL remctl
   5001:raw:0:/dev/ttyUSB1:115200 8DATABITS NONE 1STOPBIT XONXOFF LOCAL remctl
   5002:raw:0:/dev/ttyUSB2:115200 8DATABITS NONE 1STOPBIT XONXOFF LOCAL remctl

Then flash each device using the appropriate port:

.. code-block:: bash

   deploy serial rfc2217://192.168.8.1:5000  # Device on USB0
   deploy serial rfc2217://192.168.8.1:5001  # Device on USB1
   deploy serial rfc2217://192.168.8.1:5002  # Device on USB2

Security Considerations
======================

- ser2net does not provide encryption by default
- Only use on trusted local networks
- Consider using VPN if accessing over the internet
- Restrict access with firewall rules if needed

Further Resources
================

- `ser2net documentation <https://linux.die.net/man/8/ser2net>`_
- `OpenWRT ser2net package <https://openwrt.org/packages/pkgdata/ser2net>`_
- `RFC2217 specification <https://tools.ietf.org/html/rfc2217>`_


Top: `ToC <index-doc.rst>`_.
