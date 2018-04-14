# This is the autostart code for a ulnoiot node.
# Configure your devices, sensors and local interaction here.

# Always start with this to make everything from ulnoiot available.
# Therefore, do not delete the following line.
from uiot import *

# The following is just example code, adjust to your needs accordingly.

## wifi and mqtt connect are done automatically, we assume for this example
## that the mqtt node topic is myroom/test1

## Use some shields
# The onboard-led is always available.
# With this configuration it will report under myroom/test1/blue
# and can be set via sending off or on to myroom/test1/blue/test.
# from uiot.shield.onboardled import blue
# blue.high() # make sure it's off (it's reversed)

## Add some other devices (all commented out)
# Add a button with a slightly higher debounce rate, which will report
# in the topic myroom/test1/button1.
# d("button", "button1", d8, threshold=2)

# Count rising signals on d2=Pin(4) and
# report number counted at myroom/test1/shock1.
# d("trigger", "shock1", Pin(4))

## Start to transmit every 10 seconds (or when status changed).
# Don't forget (uncomment!) the run-command when things shall
# start automatically (for debugging, you want to first leave this commented).
# run(10)
