Setting the phone as the Gateway
=================================

Basic installation on the phone:
- f-droid from the web page (https://f-droid.org/)
- install termux, termux:boot, and termux:widget (all from f-droid)
- Install IoTempower on the phone in the termux terminal (https://github.com/iotempire/iotempower)

Now we need to set the permissions for accessing the phone via computer with SSH. (For Windows, you probably need to install SSH). After installing SSH, follow the instructions below.

Phone set-up
------------

You might want to change the energy settings of your phone to allow Termux to run all the time. There will be somewhere in the settings. You should let the screen lock last a little longer. Remove the PIN or face recognition.

Secure Shell (SSH)
------------------

Create in `.ssh/` a personal public key `id_rsa.pub` (and `id_rsa` private) using `ssh-keygen`, no password needed.

Now we want to transfer our public key from the computer to the phone so we can access the phone without the need to write the password every time. And it is also the only way possible.

First, find the IP address on your computer. Open terminal and type:

::

    ifconfig

or for Windows:

::

    ipconfig

or 

::

    ip a

Find the code (like this 192.168.1.144) after "inet".

Open Termux on the phone and run:

::

    scp username@ip_address_of_computer:./.ssh/id_rsa.pub .

Password: put the user password. On Windows, you should use a file manager to copy the `id_rsa.pub` file directly to a USB-connected phone. (On the phone, you might need to turn on the file transfer in the notifications).

Now we have the public key, you should run on termux (cell phone):

::

    cat id_rsa.pub >> .ssh/authorized_keys

(Attention: if you copied from Windows, you might have to copy it from a different path, probably ``cat /sdcard/id_rsa.pub >> .ssh/authorized_keys``)

Now type in Termux (cell phone):

::

    sshd

So you allow it to start the SSH server on port 8022 (to allow you to connect via network from your computer to your phone).

More info here: https://wiki.termux.com/wiki/Remote_Access

Access your phone from the Computer
-----------------------------------

Make sure your phone and computer are on the same network. Now you are able to access your phone from the computer. Find the IP of your phone by running:

::

    ifconfig

If it is not installed, run:

::

    pkg install net-tools

IoTempower normally installs it for you.

Then access your phone by running in the computer terminal:

::

    ssh 192.168.1.109 -p 8022

Then you should be able to access your phone. If it asks for a password, then something with your key exchange didn't work. Try the following: 

- Check if ``cat $HOME/.ssh/authorized_keys`` includes your `id_rsa.pub` key. If not, try the procedure above to get it in there.

- Run ``pkill sshd; chmod 600 $HOME/.ssh/authorized_keys; sshd``

- Try again

Make your phone a nice Dashboard using Node-RED
-----------------------------------------------

If you are using the Raspberry Pi as the gateway or you want to see the deployed sensors working, you can use your phone as a Dashboard with Node-RED.

On your computer, you can access the Raspberry Pi with https://gw.iotempire.net/, and you can see the graphs on the phone with Node-RED.

To do so, you can run on the phone:

::

    ssh iot@ip_address_of_pi -L 40081:localhost:40080

For example ``ssh iot@192.168.10.71 -L 40081:localhost:40080``.

Password is ``iotempire``.

Then enter the IoT environment:

::

    iot

Then start the services:

::

    web_starter

Then access Chrome and run the following address:

::

    localhost:40081

Access the Node-RED user interface page. And see the graphs.

Copy all images from the Phone to Computer using SCP
----------------------------------------------------

You need to install Termux on the phone for this. Also, you need to alter all permissions of Termux in the App info of the A3 Redmi Xiaomi phone, making all permissions allowed for Termux.

First, you need to find the IP of the phone by opening Termux and typing:

::

    ifconfig

Find the address after inet, something like: 192.168.1.255.

Then open your computer terminal and run:

::

    scp -P 8022 192.168.1.109:/data/data/com.termux/files/home/storage/dcim/Camera/'*' .


---

Quick note for Uli
===================

My Cloud Commander is version v17.4.0.
