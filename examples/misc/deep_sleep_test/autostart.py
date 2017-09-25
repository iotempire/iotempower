# deep sleep test
import machine

import time
from machine import Pin

blue=Pin(2,Pin.OUT)
for i in range(3):
	blue.low()
	time.sleep(0.5)
	blue.high()
	time.sleep(0.5)


# configure RTC.ALARM0 to be able to wake the device
rtc = machine.RTC()
rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

# check if the device woke from a deep sleep
if machine.reset_cause() == machine.DEEPSLEEP_RESET:
    print('woke from a deep sleep')

# set RTC.ALARM0 to fire after 10 seconds (waking the device)
rtc.alarm(rtc.ALARM0, 10000)

# put the device to sleep
machine.deepsleep()
