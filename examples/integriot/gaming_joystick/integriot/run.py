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


def up_cb(msg):
    inject_key(kb.up_key, msg)


def left_cb(msg):
    inject_key(kb.left_key, msg)


def down_cb(msg):
    inject_key(kb.down_key, msg)


def right_cb(msg):
    inject_key(kb.right_key, msg)


def fire_cb(msg):
    inject_key(kb.space, msg)


init("192.168.12.1")

prefix("joystick1")
subscriber("up").subscribe_change(callback=up_cb)
subscriber("down").subscribe_change(callback=down_cb)
subscriber("left").subscribe_change(callback=left_cb)
subscriber("right").subscribe_change(callback=right_cb)
subscriber("fire").subscribe_change(callback=fire_cb)

onboard_blue = switch_publisher("blue", init_state="off")

print("Keyboard injector started, press Ctrl-C to stop.")
run()
