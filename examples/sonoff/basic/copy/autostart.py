from uiot import *
import time

led = d("out", "led", onboardled, "off", "on")
# blink when starting for 2s
for _ in range(5):
    led.evaluate("on")
    time.sleep(0.2)
    led.evaluate("off")
    time.sleep(0.2)

r1 = d("out", "relais1", relais1,
       on_change=lambda t: led.evaluate(t.mapped_value()), report_change=True)
r1.evaluate("off")


def switch1_cb(t):
    if t.mapped_value() == "unpressed":
        if r1.mapped_value() == "on":
            r1.evaluate("off")
        else:
            r1.evaluate("on")


d("contact", "button", button1, report_high="unpressed", report_low="pressed",
  threshold=7, on_change=switch1_cb, report_change=False)

run()
