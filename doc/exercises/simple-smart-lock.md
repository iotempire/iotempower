# Simple Smart Lock

Attention: This excercise is sllightly outdated, but will be updated soon.

This tutorial will guide you through building a solenoid-based simple smart-lock
and allow you to control it through an mqtt server.
Make sure you document the progress so you can share your problems, success,
and modifications back with the community, teacher, or colleagues.

To build such a smart lock from the ulno development kit shields
is very straight forward.
You just need the follwing:
- Wemos D1 Mini base
- a [relay shield](../../shields/relay/README.md)
- a way to power the wemos (this might be just the usb port of your computer, 
  a phone charger battery, or a simple phone charger power supply).
  Careful, sometimes the computer usb port does not yield enough power and the
  wemos starts resetting frequently. This happens especially, when you use
  also the display shield at the same time as the relay. This usually works
  perfectly well and is recommended, if you are debugging.
- an electric lock
- a power supply triggering either the opening or the closing of the lock
  ([such a  lock](https://www.aliexpress.com/item/Electric-Strike-Door-Lock-For-Access-Control-System-New-Fail-safe-fail-secure-5YOA-Brand-New/2044042957.html?ws_ab_test=searchweb0_0,searchweb201602_5_10056_10065_10068_10055_10054_10069_10059_10078_10079_10073_10017_10080_10070_10082_10081_10060_10061_10052_10062_10053_10050_10051,searchweb201603_4&btsid=182056bb-eaa0-434c-8256-dac4c562c6d7)
  has been used successfully)
- optional: [display shield](../../shields/display/README.md)

Make sure you have configured an mqtt server (we will refer to its hostname or
ip address in the future as MQTT_SERVER) and have already set-up wifi on your
wifi network as well as toyed and understood at least the 
[devel shield](../../shields/devel/Readme.md).

1. First, just plug the relay shield into the Wemos D1 Mini base
   (if you want, you can plug the display shield in between - first
   the display, then the relay on top).
2. Power the wemos and login into your wemos via webrepl.
3. Try the following at the repl command prompt:

   ```
   import ulno_iot_relay as relay
   relay.right()
   relay.left()
   ```

4. You should have heard two clicks.
   In default (off at the beginning and later after executing relay.left) mode, 
   when the little red led on the shield
   is not lit, the center and the left (the one close to the tiny red led, the 
   wemos cable facing up, the antenna down) contacts are connected. 
   If you call relay.right, the center and the right contact are connected,
   the left is then disconnected. The red led should be on then. If you are 
   not sure about the connections, use a voltmeter to confirm the directions.
5. Unplug the wemos and wire the electric lock in connecting one cable of your
   lock power supply to the center contact of the relay, one cable to the lock
   and one other cable from the right contact to the lock (we assume that the
   lock is open, when it is powered, check this in advance else you can just
   use the left connector).
6. Plug in the lock power adapter and repeat step 3. Check after the right and
   left commands if you can open the lock (it should open after executing the
   right command).
7. Use the umqtt.simple (if necessary google a simple usage example) module 
   to write a small program (put it with webrepl into user_boot.py) 
   which subscribes to an mqtt topic.
8. Control the lock via a generic mqtt client.
9. Use [IOT Manager](https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager)
   on an Android phone and build a small gui to open and close
   your lock from your phone via your wifi network. You can find more
   documentation for IOT Manager [here](http://esp8266.ru/iotmanager/).
