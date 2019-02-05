dht(name, pin);
Other names: dht11, dht22

ds18b20(name, pin);
Other names: dallas

Create a new temperature (+ humidity) sensor device.
name will be appended to the mqtt topic and corresponding
value will be published there under a subtopic temperature
or humidity.
Currently the dht11, dht22, and ds18x20 chips are supported. Chose
the respective creation method.

Parameters:
- name: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).
- pin: the gpio pin

With .with_filter a filter function can be specified: (see uhelp filter).

Example (node name: living room/ht1):
dht(ht01, D1);

Now temperature and humidity are published as
"living room/ht1/ht01/temperature" and "living room/ht1/ht01/humidity".