# i2c connector
# can be both, sensor and actor
#
# author: ulno
# created: 2017-10-20
#

import time
from machine import I2C
from uiot.device import Device


class I2c_Connector(Device):
    BUFFER_SIZE = 36  # counter (2) + size (1) + suspend request (1) + data (32)

    def __init__(self, name, sda=None, scl=None,
                 addr=8, ignore_case=False, report_change=True):
        not_found = "i2c device not found"

        self.addr = addr
        self.ownaddr = addr
        self.count = None
        self.current_value = ""
        self.suspend_start = None
        self.suspend_time = 0
        self.msgq = None
        if type(sda) is I2C:
            i2c = sda
        else:
            i2c = I2C(sda=sda, scl=scl)

        try:
            l = i2c.scan()  # see, if you can find the dive with respective addr
        except OSError:
            print(not_found)
        else:
            if addr in l:
                self.present = True
                Device.__init__(self, name, i2c, setters={"set": self.evaluate},
                                ignore_case=ignore_case, report_change=report_change)
                self.getters[""] = self.value
            else:
                print(not_found)

    def send(self, msg):
        self.msgq = msg  # queue for sending in update
        # TODO: prevent messages from getting lost

    def _update(self):
        t = time.ticks_ms()
        # if self.suspend_start is not None \
        #        and time.ticks_diff(t,self.suspend_start) <= self.suspend_time:
        #    print("Bus access suspended.")
        if self.suspend_start is None \
                or time.ticks_diff(t, self.suspend_start) > self.suspend_time:
            self.suspend_start = None  # done waiting
            try:
                if self.msgq is not None:
                    self.pin.writeto(self.addr, self.msgq)
                    self.msgq = None
                s = self.pin.readfrom(self.addr, self.BUFFER_SIZE)
            except:
                print("Trouble accessing i2c.")
            else:
                if s[0] == 255 and s[1] == 255:  # illegal read
                    print("Got garbage, waiting 1s before accessing bus again.")
                    print("data:", s)
                    self.suspend_time = 1000  # take a break
                    self.suspend_start = time.ticks_ms();
                else:
                    count = s[0] * 256 + s[1]
                    if self.count != count:
                        l = s[2];
                        self.suspend_time = s[3];
                        # scale timer
                        if self.suspend_time & 128:
                            self.suspend_time = (self.suspend_time & 127) * 100
                            if self.suspend_time > 5000:  # should not happen
                                print("Garbage time -> waiting 1s before accessing bus again.")
                                print("data:", s)
                                self.suspend_time = 1000
                        if self.suspend_time > 0:
                            self.suspend_start = time.ticks_ms();
                            print("Bus suspend for", self.suspend_time, "ms requested.")
                        if l <= self.BUFFER_SIZE:  # discard if too big
                            self.count = count
                            self.current_value = s[4:4 + l]

    def value(self):
        return self.current_value

    def evaluate(self, msg):
        self.send(msg)
