# the devel shield
Small shield for developing and learning purpose.
It has three buttons, a red, and a yellow led.

The mapping to GPIO-ports is like the following:
- d0: 16 lower button
- d3:  0 flash and left button
- d4:  2 on board led (reversed low: on, high: off)
- d6: 12 right button
- d7: 13 red led
- d8: 15 yellow led

To use this shield execute:
```python
import ulno_net_devel as devel
```

All devices can now be accessed by their name or pin name.
Examples:
```python
devel.red.high()
devel.yellow.high()
devel.blue.low() # The on board led lights up when set to low
devel.lower_button() # will be 1 when not pressed and 0 when presssed
devel.left_button()
devel.right_button()
devel.d6() # button can also be accessed via it's pin name
```

Here some images of the shield:
[[../../doc/pics/devel_t.jpg|alt=top]]

[[https://github.com/ulno/micropython-extra-ulno/blob/master/doc/pics/devel_t.jpg?raw=true]]
