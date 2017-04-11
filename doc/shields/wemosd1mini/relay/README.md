# the relay shield
This is a ready available shield for the wemos d1 mini.

You can use this shield to switch higher voltage.
However, do not use voltages over 20V. Higher voltages can be really dangerous.
Anything with higher voltage is your own risk.

Example usage:
```python
mqtt("192.168.10.254","testfloor")
# this adds already an output object with the name relay
from ulnoiot.shield import relay
run() # now it can be addressed
```
