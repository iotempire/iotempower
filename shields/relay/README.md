# the relay shield
You can use this shield to switch higher voltage.
However, do not use voltages over 20V. Higher voltages can be really dangerous.
Anything with higher voltage is your own risk.

Example usage:
```
import ulno_iot_relay as relay
relay.right()
relay.on() # equivalent to right
relay.left()
relay.off() # equivalent to left
```
