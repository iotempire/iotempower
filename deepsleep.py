#If not working as expected blame pramodag(https://github.com/pramodag)
#measures and prints dht11 temprature and humidity sensor.
from ulnoiot import *
import machine
import dht

def main():
    if machine.reset_cause() == machine.DEEPSLEEP_RESET:
        print('woke from a deep sleep')
    else:
        print('power on or hard reset')
    wifi("iotempire-jp","internetofthings")
    d3 = dht.DHT11(machine.pin(4))
	d3.measure()
    d3.temperature()
    d3.humidity()
    # configure RTC.ALARM0 to be able to wake the device
    rtc = machine.RTC()
    rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

    # set RTC.ALARM0 to fire after 10 seconds (waking the device)
    rtc.alarm(rtc.ALARM0, 10000)

    # put the device to sleep
    machine.deepsleep()
main()
