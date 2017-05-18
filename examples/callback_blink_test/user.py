from ulnoiot import *
from ulnoiot.shield import devkit1_2led3but as d


def light_red(btn):
    if btn.mapped_value() == b"pressed":
        d.red.high()
        do_later(2,lambda _: d.red.low())

def main():
    mqtt("192.168.12.1", "callback-test")

    d.lower.set_on_change(light_red)
    ## start to transmit every 10 seconds (or when status changed)
    run(10)

main()