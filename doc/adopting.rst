Installing IoT Empower for Linux (and WSL)
============================================

The steps for WSL and Linux should be the same. For information on how to run IoT Empower on a Raspberry Pi, please go to this link:
https://github.com/iotempire/iotempower/blob/master/doc/installation.rst

1. In Linux (WSL or native), run the following commands in root (~)
- sudo apt-get update
- sudo apt install git mc mosquitto mosquitto-clients virtualenv iptables bridge-utils hostapd dnsmasq nodejs build-essential npm
- sudo npm install terminal-kit

2. Clone the IoT Empower repository using git
- git clone https://github.com/iotempire/iotempower

3. Copy examples/scripts/iot into your bin folder and adapt the path in it to reflect the location where you cloned the IoTempower 
(ex. to export IOTEMPOWER_ROOT="$HOME/iotempower" if your iotempower git directory is in root(~))

4. Run ./iot, you should get a welcome message. Accept the installation of any extra packages. After the installation, you can run just iot 
to see if you have everything installed correctly.

Pre-flashing Wemos D1 Mini
============================================

Note that some nodes might be already pre-flashed, but that should not be an issue.

1. Add port permissions for less hassle (replace your-username with the username you chose/have).
- sudo usermod -a -G dialout your-username

2. Restart PC (Linux) or terminal (WSL).

3. Start iot empower

4. Plug in your Wemod D1 Mini if you haven't already.

For Windows Users

- Make note of the COM port that the Device Manager Shows (for example (COM4))

- Run the following command, but replace the y with the number after COM (in the above example y = 4)
	pre_flash_wemos ttySy force

For Linux Users

- pre_flash_wemos

If you are getting a /dev/ttySy permission denied error, add yourself to the dialout group or run the command with sudo.

If you are getting that /dev/ttySy error related to wemos not being plugged in, most likely you have incorrectly specified which ttySy port 
Wemos is connected to.

Wait for the pre_flash_wemos command to finish, it should take about 3:30 minutes.

5. Now, prepare the Wemos for the next exercise. Attach the Button Shield to your Wemos D1 Mini.
Make sure that the pins align with the Wemos.

Managing Repo
============================================

We will now setup our own folder so we can modify what our Wemos D1 mini can do.
For that, we need to copy one of folders in the git repository outside of the repository.

You can replace Demo with anything you want.
1. cp -R iotempower/lib/system_template ~/Demo

Then change the node_template folder name into something more appropriate.
- mv ~/Demo/node_template ~/Demo/test

Now, we need a few IP's to which our Wemos can connect to.

For Windows

- Open the Command Prompt
- ipconfig
- Look for the Default Gateway IP and IPv4 Address

For Linux

- Open Terminal
- ip a
- Look for inet scope global dynamic ip and default gateway.


2. Copy iotempower.example.conf in /iotempower/etc to just iotempower.conf
- cp ~/iotempower/etc/iotempower.conf.example ~/iotempower/etc/iotempower.conf

3. Change parameters of the iotempower.conf to the following (not that {values} need to changed to what you found in the above steps or to whatever your SSID is)
- nano iotempower.conf
```
IOTEMPOWER_AP_NAME={SSID}
IOTEMPOWER_AP_ADDID=no
IOTEMPOWER_AP_CHANNEL=random
IOTEMPOWER_AP_HIDDEN=no

IOTEMPOWER_AP_IP={Default Gateway IP}

IOTEMPOWER_AP_HOSTNAME = ""

IOTEMPOWER_MQTT_HOST={IPv4 IP}           //don't forget to uncomment this line (remove #)
```

4. Go to the folder we copied earlier
- cd ~/Demo

5. (Optional, Recommended) You can see some documentation for more information by running the following commands
- iot doc make
- iot doc serve

If both of these commands worked, you should now be able to go to localhost:8001 in your browser and see some documentation.

We will now write a small script for our Wemos D1 Mini.

6. cd ~/Demo/test

7. nano setup.cpp

Add the following line to the end of the setup.cpp file
```
input(button1, D3, "released", "pressed");
```

Deployment
================================================

1. In the /test folder, run the following command
	deploy serial ttySy

If you forgot the y value, check the Device Manager -> Ports
If you are on native Linux, you might get away with just 'deploy serial'
	

2. Open 3 more Linux terminals (make sure you are in iotempower for all 3 terminals). Run a singular command on one terminal, ex
- Terminal 1: mqtt_broker
- Terminal 2: console_serial ttySy
- Terminal 3: mqtt_listen

3. Now press the button that is attached to your Wemos D1 Mini. If you did everything
correctly, you should see "released" , "pressed", on the terminal where you ran console_serial.


DONGLE FLASHING and ADOPTING
===============================================

You may remove the Button Shield from your Wemos D1 Mini and attach a OLED Screen Shield to it now.

NOTE! You don't have to attach the Screen to your Wemos D1 Mini if you don't have one and doing it outside of the lab.

We will make one Wemos into a Dongle and put a second one into Adoption Mode

Connect one of the Wemos's aand run the following command

1. dongle flash ttySy

The OLED Screen on the Wemos D1 Mini should show "No gateway detected" if it worked.

Run the following command and put the Wemos aside while still having it connected to power.

2. dongle daemon ttySy

Connect the second Wemos that is not the Dongle, attach an OLED Shield to it, and put it into adoption mode by resetting it and double clicking on the 
left button of the OLED Shield.
You should see the diplay showing the amount of seconds you have left to ADOPT it. If you don't see anything on the display, 
try the following:

- Make sure the second Wemos has iotempower code in it. To ensure that, run pre_flash_wemos on it
- It might be a faulty Wemos/OLED shield, try another Wemos.
- Try resetting the Wemos.

If your screen is showing that the Wemos is in adoption, go to your node folder and run the following command.
3. iot menu

4. Press Adopt and look for your Wemos. Your OLED Shield should show you the amount of long and short blinks your personal Wemos has. If you do 
not have an OLED Shield, count the amount of LED blinks.

After that, run the following command on the FLASHED Wemos 

