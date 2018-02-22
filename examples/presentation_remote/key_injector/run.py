# install from https://github.com/PyUserInput/PyUserInput
from pykeyboard import PyKeyboard

from integriot import *

kb = PyKeyboard()


def inject_key(key, msg):
    global kb
    if msg == "1":
        kb.press_key(key)
    else:
        kb.release_key(key)


def back_cb(msg):
    inject_key(kb.left_key, msg)

def forward_cb(msg):
    inject_key(kb.space, msg)


init("192.168.12.1")

prefix("remote1")
sensor("forward").add_callback_change(callback=forward_cb)
sensor("back").add_callback_change(callback=back_cb)

onboard_blue = led("blue", init_state="off")

print("Keyboard injector started, press Ctrl-C to stop.")
run()
