servo
=====

..  code-block:: cpp

    servo(name, pin, min_us=600, max_us=2400, 
        turn_time_ms=700);

Create a new servo motor controller.
``name`` will be appended to the mqtt topic and a set appended to
send angles to.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``pin``: the gpiopin

- ``freq``, ``min_us``, ``max_us``: 
   values controlling the start and end motor timing
   settings

- ``turn_time_ms``: how long to give the motor in ms to reach its position before
  stopping power

Example
-------

..  code-block:: cpp

    servo(m1, D4);

Now the motor can be controlled via sending to the mqtt-broker
to the topic "node_topic/m1/set" the angle to turn the motor to.

