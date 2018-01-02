# HCSR04 Device
# author: cmuck, ktakac, ulno, Gabriel Schützeneder
#
# Sources:
#  - Roberto Sánchez - https://github.com/rsc1975/micropython-hcsr04
#  - http://www.micropik.com/PDF/HCSR04.pdf
# created: 2017-10-23
#
# Driver to use the ultrasonic sensor HC-SR04.
# The sensor range is between 2cm and 4m.
#
# The timeouts received listening to echo pin are converted to OSError('Out of range')
#
# adapted by Gabriel Schützeneder, cmuck, and ulno in order to function with ulnoiot.
#
# start with hcsr04("name", pin_trigger, pin_echo)
# pins can be either GPIO Ports or Pin-objects

import machine
from machine import Pin
from uiot.device import Device
from time import sleep_us, ticks_us, ticks_ms, ticks_diff


class Hcsr04(Device):
    INTERVAL = 50  # wait how many ms until next measurement?

    def __init__(self, name, trigger_pin, echo_pin,
                 echo_timeout_us=30000, precision=10,
                 on_change=None, report_change=True):
        # trigger_pin: Output pin to send pulses
        # echo_pin: Readonly pin to measure the distance.
        #           The pin should be protected with 1k resistor
        # echo_timeout_us: Timeout in microseconds to listen to echo pin.
        # By default is based in sensor limit range (4m)
        self.current_value = 0
        self._last_measured = 0
        if type(trigger_pin) is not Pin:
            trigger_pin = Pin(trigger_pin)
        self.trigger_pin = trigger_pin
        if type(echo_pin) is not Pin:
            echo_pin = Pin(trigger_pin)
        self.echo_pin = echo_pin
        self.precision = precision
        trigger_pin.init(Pin.OUT)
        trigger_pin.off()
        echo_pin.init(Pin.IN)
        echo_pin.init(Pin.OPEN_DRAIN)
        self.echo_timeout_us = echo_timeout_us
        self._distance = self._measure()
        Device.__init__(self, name, (trigger_pin, echo_pin),
                        getters={"": self.value},
                        on_change=on_change,
                        report_change=report_change)

    def value(self):
        return self._distance

    def _update(self):
        if ticks_diff(ticks_ms(), self._last_measured) > self.INTERVAL:
            value = self._measure()
            if abs(value - self._distance) >= self.precision:
                self._distance = value
            self._last_measured = ticks_ms()

    def _send_pulse_and_wait(self):
        # Send the pulse to trigger and listen on echo pin.
        # We use the method `machine.time_pulse_us()` to
        # get the microseconds until the echo is received.
        self.trigger_pin.value(0)  # Stabilize the sensor
        sleep_us(5)
        self.trigger_pin.on()
        # Send a 10us pulse.
        sleep_us(10)
        self.trigger_pin.off()
        # try:
        #     pulse_time = machine.time_pulse_us(self.echo_pin, 1, self.echo_timeout_us)
        #     return pulse_time
        # except OSError as ex:
        #     if ex.args[0] == 110: # 110 = ETIMEDOUT
        #         return -1 # out of range
        #     raise ex

        start = ticks_us()
        while not self.echo_pin():
            t = ticks_us()
            if ticks_diff(t, start) > self.echo_timeout_us:
                print("HCR04: timeout")
                return -1
        start = ticks_us()
        while self.echo_pin():
            t = ticks_us()
            if ticks_diff(t, start) > self.echo_timeout_us:
                print("HCR04: timeout")
                return -1
        delta = ticks_diff(ticks_us(), start)
        return delta

    def _measure(self):
        # Get the distance in millimeters without floating point operations.
        pulse_time = self._send_pulse_and_wait()

        # To calculate the distance we get the pulse_time and divide it by 2
        # (the pulse walk the distance twice) and by 29.1 becasue
        # the sound speed on air (343.2 m/s), that It's equivalent to
        # 0.34320 mm/us that is 1mm each 2.91us
        # pulse_time // 2 // 2.91 -> pulse_time // 5.82 -> pulse_time * 100 // 582
        if pulse_time is not None:
            return pulse_time * 100 // 582
        else:
            return -1
