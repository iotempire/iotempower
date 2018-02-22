from uiot import *

d("led", "blue", onboardled, "off", "on")

d("input","up", d1, 0, 1)
d("input","left", d2, 0, 1)
d("input","down", d3, 0, 1)
d("input","right", d6, 0, 1)
d("input","fire", d7, 0, 1)

run()
