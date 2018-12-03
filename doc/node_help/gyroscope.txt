gyro6050(name)[.i2c(sda,scl)][.with_address(i2c_address)];

Other names:
gyro6050

Create a new gyroscope sensor device.
name will be appended to the mqtt topic and the current angles will be
published under the subtopic ypr and accelration values under the suptopic acc.

Parameters:
- name: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).
- .i2c(sda,scl): move i2c to given ports - if not specified uses default 
  i2c ports
- .with_address(i2c_address): the i2c_address is the address of the gyroscope.

With .with_filter a filter function can be specified: (see uhelp filter).

Example (node name: mynode):
gyro6050(g1);

Now angles and acceleration are published as "mynode/g1/ypr" and 
"mynode/g1/acc".