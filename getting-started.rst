How to get started with ulnoiot on the Wemos D1 mini
====================================================

1. Checkout https://github.com/ulno/ulnoiot
  - If you just want read-only access just type in a folder of your choice:
    ``git clone https://github.com/ulno/ulnoiot``
  - If you are a ulnoiot developer, use
    ``git clone git@github.com:ulno/ulnoiot``

2. If you want fast shell access to the ulnoiot environment, add the 
   following alias to your shell configuration (like .bashrc).

   Make sure to adjust ULNOIOTPATH ot the path where you downloaded
   ulnoiot.
   
   alias ulnoiot='cd ULNOIOTPATH; . bin/activate'

3. If you installed the alias, just type ulnoiot and follow the
   instructions. If not activate the enverinmont in sourcing bin/activate
   as ``. bin/activate``.

4. If you need to flash a wemos, just type flash_wemosd1mini

5. Install or update the ulnoiot software with deploy_wemosd1mini

6. Access the command prompt with console (if only a wemos is connected
   the serial port will be dicovered automatically else supply it or
   an IP address and password as paramaters).

7. You can now inside the console configure the wifi with calling the
  ``wifi`` command and change the webrepl login password from
  default ulnoiot to somethign else with ``import webrepl_setup``

If something gets stuck, try to power cycle the wemos d1 mini.

   