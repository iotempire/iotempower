==========
First Node
==========

For this section, we assume that you have successfully set-up UlnoIoT
in the Raspberry Pi. 
UlnoIoT supports the following microcontroller boards:
  
  - Wemos D1 Mini Mini
  - NodeMCU
  - Espresso Lite V2
  - Sonoff and Sonoff Touch
  - There is an esp8266 generic option for other esp8266-based boards.

You will need a smart phone or a computer for this first node setup. 

If you use a Wemos D1 Mini Mini as your first node (this is the default),
no change is necessary. If you want to use a NodeMCU, change the config to NodeMCU.

TODO: Insert the link to the serial flash page! If you are in the classroom your devices will be pre-flashed with UlnIoT OTA on them. 
If not please go to this page `First Flash page`_ on your Raspberry Pi to pre-flash your device. 



Setting up the WiFi credentials on the node
-------------------------------------------

- TODO: Insert image of the button shield! Connect the button to the D3 and ground or a button shield (see image) on top of the Wemos D1 Mini.

- Power up your microcontroller via battery or usb power supply. The onboard_led
  will blink for 5 seconds. While that is happening press the button 3 times. The blinking 
  pattern should change. If the Led is continously on, restart your board by powering it off
  and back on and try the process again. 
  
- If you have done previous step correctly, the onboard_led should be now blinking
  in a unique patttern (for exemple 3 long and 2 short blinks). Pay attention to
  pattern as it will be use to identify the boards WiFi network.

- Adopting process for one Node from the phone. Describe 

- Change the WiFi credentials to match the WiFi of the Raspberry Pi's WiFi 
  network credentials the ``WiFi Name`` and ``Password`` that was setup 
  previously on `<quickstart-pi.rst>`_

- Restart your microcontroller by powering it off and back on again.

- If you have not done already, connect now your computer (laptop or 
  desktop pc) to Pi's WiFi network (use the password set as before 
  for ``Password``). 

- You can now connect to UlnoIoT via a web browser.
  
  - Point your browser at https://ulnoiotgw (or https://ulnoiotgw.local). 
    Accept the security exception for the locally generated security 
    certificate. 

  - If you are asked for a user, use ``ulnoiot``, if you are asked for a password
    use ``iotempire``.
    
  - TODO: provide image of home page! You should now see the home page for UlnoIoT.

- Click on the link `Iot system exemple 
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/>`_ to navigate to
  the folder where the templates are.

- TODO: provide image of the Iot system exempla page! You should be seen a page like the one bellow. 

- Navigate to the `Node 1 
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/node1/>`_ folder. 

- TODO: provide image of the Folder 1! You should be seen a page like the one bellow. 

- TODO: provide image of the weird button! When inside the `folder 1`_ click on the console button located on the bottom
  right corner your screen.  

- TODO: provide image of the button menu for initializing! You will now see a menu link this one on the image bellow.

- Click on the button initialize.

- 



- Consider to configure  etc/ulnoiot.conf
  and run ``accesspoint`` and ``mqtt_broker``. If you installed from the
  Raspberry Pi image, this should not be necessary as they are started
  automatically.

- Copy the folder ``lib/system_templates`` to a project directory,
  you can rename
  system_templates to a project name (i.e. iot-test-project)

- Rename the included node_template to a name for the node you want to
  configure (i.e. onboard_blinker)

- Adapt and configure system.conf and node.conf. Especialy make sure to add the
  correct board in node.conf. 

- Now change into your node directory, connect an (only one) esp8266 based microcontroller
  to your pc or raspberry/orange pi and type ``initialize serial``. This flashes and
  pre-configures the device. If you use the inbuilt WiFi configuration (like
  described in uhelp WiFi), just use ``initialize``
  to adopt the node via the network.

- If you like, access the debug console with ``console_serial`` (if only one esp is connected
  the serial port will be discovered automatically else supply it as usb1 or 
  acm2).

If something gets stuck, try to power cycle the esp8266.

``initialize`` sets up your WiFi based on the settings in system.conf and also
encrypts the network connection.

At an ulnoiot-command prompt try typing ``uhelp`` and check the small manual.

Take a look at the setup.cpp in your node-folder and enable the onboardled.

Try also ``uhelp setup.cpp`` at the ulnoiot prompt.


