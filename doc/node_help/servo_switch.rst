servo_switch
============

..  code-block:: cpp

    servo_switch(name, pin, 
        on_angle, off_angle, return_angle/*, on_command is by default "on"*/ /*, off_command is by default "off"*/
        /*, turn_time_ms is by default 700*/
        /*, min_us is by default 600*/ /*, max_us is by default 2400*/); /* replace one full comment with , "myvalue" (for strings) or , 800 (for numbers) if you want to change the defaults. */

Create a new servo motor controller acting as a switch.
name will be appended to the mqtt topic and a set appended to
send commands to.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via ``IN(name)``.

- ``pin``: the gpiopin

- ``on_angle/off_angle``: respective angles for on/off postions

- ``return_angle``: a medium angle to return to after pushing to on/off position
  (if not given as parameter, this feature is not used and motor does not
  return here)

- ``on_command/off_command``: what needs to be sent to turn the motor to on/off
  position

- ``turn_time_ms``: how long to give the motor in ms to reach its position before
  stopping power

- ``min_us``, ``max_us``: values controlling the motor settings

Example
-------

..  code-block:: cpp

    servo_switch(m1, D4, 0, 180, 90);

Now the motor can be controlled via sending to the mqtt-broker
to the topic ``node_topic/m1/set`` the command on or ``off``.
