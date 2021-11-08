DONGLE FLASHING and ADOPTING
============================

If installed, you may remove the Button Shield from your Wemos D1 Mini and attach a OLED Screen Shield to it now.

NOTE! You don't have to attach the Screen to your Wemos D1 Mini if you don't have one and doing it outside of the lab.

We will make one Wemos into a Dongle and put a second one into Adoption Mode

Connect one of the Wemos's and run the following commands

1. ``dongle flash ttySy``

   The OLED Screen on the Wemos D1 Mini should show "No gateway detected" if it worked.

   Run the following command and put the Wemos aside while still having it connected to power.

2. ``dongle daemon ttySy``

   Connect the second Wemos that is not the Dongle, attach an OLED Shield to it, and put it into adoption mode by resetting it and double clicking on the 
   left button of the OLED Shield.
   You should see the diplay showing the amount of seconds you have left to ADOPT it. If you don't see anything on the display, 
   try the following:

   - Make sure the second Wemos has iotempower code in it. To ensure that, run pre_flash_wemos on it

   - It might be a faulty Wemos/OLED shield, try another Wemos.

   - Try resetting the Wemos.

   If your screen is showing that the Wemos is in adoption, go to your node folder and run the following command.

3. ``iot menu``

4. Press Adopt and look for your Wemos. Your OLED Shield should show you the amount of long and short blinks your personal Wemos has. If you do 
   not have an OLED Shield, count the amount of LED blinks.


