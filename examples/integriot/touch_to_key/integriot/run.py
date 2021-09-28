MQTT_HOST = "192.168.12.1"
CONTROL_NODE = "touch01"

# install pynput: https://pynput.readthedocs.io/en/latest/
from pynput import keyboard

kbc = keyboard.Controller()
key=keyboard.Key

KEYBOARD_MAP = {
    0:key.left,
    1:key.right,
    2:key.up,
    3:key.down,
    4:key.space,
    5:"a",
    6:"d",
    7:"w",
    8:"s",
    9:"e",
}

state_map={}


from integriot import *



def inject_key(key, msg):
    global kbc
    if key is not None:
        if msg == "1":
            kbc.press(key)
        else:
            kbc.release(key)



def receive_keys_cb(msg):
    pos=0
    for c in msg[::-1]:
        if pos in KEYBOARD_MAP:
            if pos in state_map:
                if state_map[pos] != c:
                    inject_key(KEYBOARD_MAP[pos], c)
            else:
                inject_key(KEYBOARD_MAP[pos], c)
            state_map[pos] = c
        pos += 1


init(MQTT_HOST)

prefix(CONTROL_NODE)
subscriber("cap").subscribe_change(callback=receive_keys_cb)

# onboard_blue = switch_publisher("blue", init_state="off")

print("Keyboard injector started, press Ctrl-C to stop.")
run()
