# user.py is the autostart code for a ulnoiot node.
# Configure your devices, sensors and local interaction here.

# Always start with this to make everything from ulnoiot available.
# Therefore, do not delete the following line.
from ulnoiot import *

# The following is just example code, adjust to your needs accordingly.

# wifi and mqtt connect are done automatically, we assume for this example
# the following configuration.
# mqtt("ulnoiotgw", "myroom/test1")

## Use some shields
# The onboard-led is always available.
# With this configuration it will report under myroom/test1/blue
# and can be set via sending off or on to myroom/test1/blue/test.
from ulnoiot.shield.onboardled import blue

blue.high()  # make sure it's off (it's reversed)

## Add some other devices
# Add a button with a slightly higher debounce rate, which will report
# in the topic myroom/test1/button1.
button("b1", d6, pullup=False, threshold=2)

# Count rising signals on d2=Pin(4) and
# report number counted at myroom/test1/shock1.
# trigger("shock1",Pin(4))

## Start to transmit every 10 seconds (or when status changed).
# Don't forget the run-comamnd at the end.
run(5)
