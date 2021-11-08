Dongle Preparation and Node Adoption
====================================

If installed, you may remove the Button Shield from your Wemos D1 Mini and attach a OLED Screen Shield to it now.

NOTE! You don't have to attach the Screen to your Wemos D1 Mini if you don't have one and doing it outside of the lab.

We will make one Wemos into a Dongle and put a second one into Adoption Mode

Connect one of the Wemos's and run the following commands (in the iot environment):

1. Creating the dongle:

   Linux: ``dongle flash``
   Windows WSL 1: ``dongle flash ttySy``

   The OLED Screen on the Wemos D1 Mini should show "No gateway detected" if it worked.

   Run the following command and put the Wemos aside while still having it connected to power.


2. Run the dongle daemon to allow to use adopt on the computer:
   
   ``dongle daemon ttySy``

3. Enable adoption mode:
   
   Now, we will put another/a second Wemos (the one that is not the dongle 
   and has been flashed or has been pre_flashed previously with IoTempower)
   into adoption mode (for pre flashing check `here <pre_flash.rst>`__).

   Attach the OLED display or button shield to the second Wemos,
   then power it (it can be powered from any usb power outlet, 
   it does not need to be connected to your PC). 
   
   When it is powering on, it flashes the onboard led in a regular
   blinking pattern for 5s.

   During these 5s press the button (left button on display shield) twice.

   If you connected the display ...

      You should see the 
      display showing the amount of seconds you have left to ADOPT it.
      If you don't see anything on the display, 
      try the following:

      - Try resetting the Wemos.

      - Make sure the second Wemos has iotempower code in it.
        To ensure that, run `pre-flash <pre_flash.rst>`__ it.

      - It might be a faulty Wemos/OLED display shield,
        try another Wemos/OLED display shield.

      If your screen is showing that the Wemos is in adoption, go to your node folder and run the following command.
   
   If you connected the button or display, you should see the onboard led
   LED blinking in a morse pattern (a couple of long and a couple of short blinks). 
   Count these and remember for later.

4. Run ``iot menu``

5. Press Adopt and look for your Wemos. Your OLED Shield should
   show you the amount of long and short blinks your personal Wemos has.
   If you do not have an OLED Shield, count the amount of LED blinks.


