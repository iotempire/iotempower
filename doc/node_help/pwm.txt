pwm(name, pin, frequency=1000);

Control the given pin with pulse width modulation (PWM).
A different frequency can be chosen for each port and even changed at runtime.
Frequency is limited to 5000.

Parameters:
- name: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).
- pin: the gpio pin to control with PWM
- frequency: the frequency for PWM (up to 5000)

Pwm allows to call set to set a duty cycle from 0 to 1023
(with 1023 meaning 100%).
The frequency can be set via the suptopic frequency/set

Example:
pwm(blue, ONBOARDLED, 2000);

Now you can set the brightness of onboardled to 50% sending 512 to
[nodename]/blue/set
