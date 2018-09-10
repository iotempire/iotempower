from uiot import *

d("led", "blue", onboardled, "off", "on")
d("button", "forward", d3, 0, 1)
d("button", "back", d4, 0, 1)

run()
