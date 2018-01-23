from uiot import *

led = d("out", "led", onboardled, "off", "on")
# blink when starting for 2s
for _ in range(5):
    led.evaluate("on")
    time.sleep(0.2)
    led.evaluate("off")
    time.sleep(0.2)

# TODO: switch leds corresponding to status of relay

r1 = d("out", "relais1", relais1, report_change=True)
r1.evaluate("off")

r2 = d("out", "relais2", relais2, report_change=True)
r2.evaluate("off")


def switch1_cb(t):
    if t.mapped_value() == "unpressed":
        if r1.mapped_value() == "on":
            r1.evaluate("off")
        else:
            r1.evaluate("on")


def switch2_cb(t):
    if t.mapped_value() == "unpressed":
        if r2.mapped_value() == "on":
            r2.evaluate("off")
        else:
            r2.evaluate("on")


d("contact", "touch1", button1, report_high="unpressed", report_low="pressed",
  threshold=7, on_change=switch1_cb, report_change=False)
d("contact", "touch2", button2, report_high="unpressed", report_low="pressed",
  threshold=7, on_change=switch2_cb, report_change=False)

run()
