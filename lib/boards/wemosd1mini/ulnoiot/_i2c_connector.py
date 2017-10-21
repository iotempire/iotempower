# i2c conenctor
# can be both, sensor and actor
#
# author: ulno
# created: 2017-10-20
#

import time
from machine import I2C
from ulnoiot.device import Device

class I2cConnector(Device):

    BUFFER_SIZE=64

    def __init__(self, name, sda=None, scl=None,
                 addr=8, ignore_case=False, report_change=True):
        not_found = "i2c device not found"

        self.addr = addr
        self.count = None
        self.current_value = ""
        if type(sda) is I2C:
            i2c = sda
        else:
            i2c = I2C(sda=sda, scl=scl)        # test if lcd is responding
        try:
            l = i2c.scan()

        except OSError:
            print(not_found)
        else:
            if addr in l:
                self.present = True
                Device.__init__(self, name, i2c, setters={"set":self.evaluate},
                                ignore_case=ignore_case,report_change=report_change)
                self.getters[""]=self.value
            else:
                print(not_found)

    def send(self, msg):
        self.pin.writeto(self.addr,msg)

    def _update(self):
        s = self.pin.readfrom(self.addr,self.BUFFER_SIZE)
        count = s[0]*256+s[1]
        if self.count is None  \
                or count > self.count \
                or abs(self.count-count) > 255:
            self.count = count
            self.current_value = s[3:3+s[2]]


    def value(self):
        return self.current_value

    def evaluate(self, msg):
        self.send(msg)