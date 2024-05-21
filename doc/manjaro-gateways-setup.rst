********************************
Set up an accesspoint on linux:
********************************

**The following guide is done on Manjaro distribution.**
**Could vary for other Linux distributions.**

#. Open system settings > connections > add new connection > add wi-Fi (shared).
#. In general configuration. Rename the connection **iot hotspot** and say connect automatically.
#. Invent an SSID. ie iotempire-teamname
#. For band, choose 2.4 Ghz
#. Select a channel that is not yet selected in your network.
#. Restrict to wlp1s0 (could vary).
#. On wi-Fi security select WPA/WPA2Personal. Choose a password with at least 8        characters. i.e. iotempire
#. Go to IPV6 and select disable. (For simplicity)
#. finish and don’t enter a password. Hit okay.
#. Open your Bash terminal and run the command::

	nmcli c modify “iot hotspot” 802-11-wireless-security.pmf 1

#. Test that you can hotspot internet from Ethernet cable and USB tethering from your  phone.
#. Make it a gateway:With Add/remove software app, install webhook and httpie and optional Arduino IDE 2 (and use Manjaro Setting Manager to add uucp to the groups of the iot user and then log out and log in again).

**Finally**:
Fix the gateway IP so that the MQTT broker can automatically detect it. In the      terminal::

    sudo -s
    cd /etc/NetworkManager/system-connections

* Find your hotspot .nmconnection file and enter to edit it. Use micro for example: micro yourhotspotxxxx.nmconnection Under ipv4 you can define the address1=ip/subnetmask,gateway.

It should look like this::

	[ipv4]
	method=shared
	address1=192.168.12.1/24,192.168.12.1

* Save it and in a new terminal you have to restart the NetworkManager systemctl restart NetworkManager Now your gateway IP should be fix to our default 192.168.12.1 and our mqtt broker will automatically detect it in the IoT enviroment by running only::

	iot
	mqtt_starter


In case of gateway IP problems
===============================

Automatic IP detection for MQTT broker::

    sudo -s
    cd /etc/NetworkManager/system-connections

Find your hotspot.nmconnection file and edit it. Under ipv4 section you can define the address1=ip/subnetmask/gateway. It should look something like this::
	[ipv4]
    	method=shared
    	address1=192.168.12.1/24,192.168.12.1

Save it and restart the Network manager with::

    systemctl restart NetworkManager

Now your gateway IP should be fixed to our default 192.168.12.1 and our mqtt broker will automatically detect it and the IoT environment by running::

    iot
    mqtt_starter
