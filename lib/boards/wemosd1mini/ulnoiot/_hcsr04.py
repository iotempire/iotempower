# HCSR04 Device
# author: cmuck, ktakač, ulno, Gabriel Schützeneder
#
# Sources:
#  - Roberto Sánchez - https://github.com/rsc1975/micropython-hcsr04
#  - http://www.micropik.com/PDF/HCSR04.pdf
# created: 2017-10-23
#
#Driver to use the untrasonic sensor HC-SR04.
#The sensor range is between 2cm and 4m.
#
#The timeouts received listening to echo pin are converted to OSError('Out of range')
#
#adapted by Gabriel Schützeneder, cmuck, and ulno in order to function with ulnoiot.
#
# start with hcsr04("name", pin_trigger, pin_echo)
# pins can be either GPIO Ports or Pin-objects

import machine
from machine import Pin
from ulnoiot.device import Device
from time import sleep_us, ticks_us, ticks_ms, ticks_diff

####### HCSR04 Distance sensor
class HCSR04(Device):
    INTERVAL=5  # wait how many ms until next measurement?
    
    def __init__(self, name, trigger_pin, echo_pin,
                 echo_timeout_us=500 * 2 * 30, precision=1,
                 on_change=None, report_change=True):
        # trigger_pin: Output pin to send pulses
        # echo_pin: Readonly pin to measure the distance. The pin should be protected with 1k resistor
        # echo_timeout_us: Timeout in microseconds to listen to echo pin.
        # By default is based in sensor limit range (4m)
        self.current_value = 0
        if type(trigger_pin) is not Pin:
            trigger_pin=Pin(trigger_pin)
        self.trigger_pin = trigger_pin
        if type(echo_pin) is not Pin:
            echo_pin=Pin(trigger_pin)
        self.echo_pin = echo_pin
        self.precision = precision
        self.trigger_pin.init(Pin.OUT,pull=None)
        self.echo_pin.init(Pin.IN,pull=None)
        self._distance=None
        self._measure()
        Device.__init__(self, name, echo_pin,
                        getters={"": self.value},
                        on_change=on_change,
                        report_change=report_change)

    def value(self):
        return self._distance

    def _update(self):
        if ticks_diff(ticks_ms,self._last_measure) > self.INTERVAL:
            value = self._measure()
            if abs(value - self._distance) >= self.precision:
                self._distance = value

    def _send_pulse_and_wait(self):
        # Send the pulse to trigger and listen on echo pin.
        # We use the method `machine.time_pulse_us()` to
        # get the microseconds until the echo is received.
        self.trigger.value(0) # Stabilize the sensor
        sleep_us(5)
        self.trigger.value(1)
        # Send a 10us pulse.
        sleep_us(10)
        self.trigger.value(0)
        try:
            pulse_time = machine.time_pulse_us(self.echo, 1, self.echo_timeout_us)
            return pulse_time
        except OSError as ex:
            if ex.args[0] == 110: # 110 = ETIMEDOUT
                raise OSError('Out of range') # TODO: raise something else?
            raise ex

    def _measure(self):
        # Get the distance in millimeters without floating point operations.
        pulse_time = self._send_pulse_and_wait()

        # To calculate the distance we get the pulse_time and divide it by 2
        # (the pulse walk the distance twice) and by 29.1 becasue
        # the sound speed on air (343.2 m/s), that It's equivalent to
        # 0.34320 mm/us that is 1mm each 2.91us
        # pulse_time // 2 // 2.91 -> pulse_time // 5.82 -> pulse_time * 100 // 582
        self._distance = pulse_time * 100 // 582
        self._last_measure=ticks_ms()
