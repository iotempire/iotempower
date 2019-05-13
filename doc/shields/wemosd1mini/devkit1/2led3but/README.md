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
from iotempower.shield import devkit_2led3but
```

Here some images of the shield:

![top](devel_t.jpg)

![button](devel_b.jpg)
