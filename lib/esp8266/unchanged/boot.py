# This file is executed on every boot (including wake-boot from deepsleep)
# import esp
# esp.osdebug(None)
import gc
# import webrepl
import esp

from machine import UART
uart0 = UART(0)

esp.osdebug(None)
import machine

machine.freq(160000000)  # fast default

from machine import UART
uart0 = UART(0)

import ulnoiot._wifi

ulnoiot._wifi.connect()

# webrepl.start()
gc.collect()
