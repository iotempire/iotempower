"""
Driver for the MPR121 capacitive touch sensor.
This chip is on the LCD32MKv1 skin.

adapted by [ulno] for esp8266 micropython from http://micropython.org/resources/examples/mpr121.py

thepin "add" is usually shortened to ground, resulting in the 
i2c address 90=0x5e

"""

# import pyb
from machine import I2C, Pin

# register definitions
TOUCH_STATUS = const(0x00)
ELE0_FILT_DATA = const(0x04)
ELE0_TOUCH_THRESH = const(0x41)
DEBOUNCE = const(0x5b)
ELEC_CONFIG = const(0x5e)
ELEC_ENABLE_DEFAULT=8 # how many electrodes are touch bydefault

class MPR121:
    def __init__(self, i2c_sda_pin, i2c_scl_pin):
        self.i2c = I2C(sda=Pin(i2c_sda_pin), scl=Pin(i2c_scl_pin))
        self.addr = 90 # I2C address of the MPR121
        # enable ELE0 - ELE3
        self.enable_elec(ELEC_ENABLE_DEFAULT)

    def enable_elec(self, n):
        """Enable the first n electrodes."""
        buf = bytearray((n & 0xf,))
        self.i2c.writeto_mem(self.addr, ELEC_CONFIG, buf)

    def threshold(self, elec, touch, release):
        """
        Set touch/release threshold for an electrode.
        Eg threshold(0, 12, 6).
        """
        buf = bytearray((touch, release))
        self.i2c.writeto_mem(self.addr, ELE0_TOUCH_THRESH + 2 * elec, buf)

    def debounce(self, touch, release):
        """
        Set touch/release debounce count for all electrodes.
        Eg debounce(3, 3).
        """
        v = (touch & 7) | (release & 7) << 4 
        buf = bytearray((v,))
        self.i2c.writeto_mem(self.addr, DEBOUNCE, buf)

    def touch_status(self, elec=None):
        """Get the touch status of an electrode (or all electrodes)."""
        status = self.i2c.readfrom_mem(self.addr, TOUCH_STATUS, 2)
        status = status[0] | status[1] << 8
        if elec is None:
            return status
        else:
            return status & (1 << elec) != 0

    def elec_voltage(self, elec):
        """Get the voltage on an electrode."""
        data = self.i2c.readfrom_mem(self.addr, ELE0_FILT_DATA + 2 * elec, 2)
        return data[0] | data[1] << 8
