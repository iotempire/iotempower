# short example how to use integriot and alsaaudio to control the volume via mqtt
# author: ulno

# TODO: turn these into arguments
mqtt_server = "homeaut"
sink_name = 'Altec Lansing XT2 - USB Audio'
location = "living-room"
device = "audio01/volume"

import alsaaudio  # https://pypi.org/project/pyalsaaudio/
from integriot import *
from time import sleep

# find card
card = -1
for index in alsaaudio.card_indexes():
    if sink_name in alsaaudio.card_name(index):
        card = index
        break
mixers = alsaaudio.mixers(cardindex=card)
sink = None
for name in ["Master", "PCM", "Speaker", "Front","FM"]:
    if name in mixers:
        sink = alsaaudio.Mixer(name, cardindex=card)
        break

if sink is None:
    print("Mixer not found")
    exit(1)
        
# mqtt setup
init(mqtt_server)
prefix(location)
p = publisher(device)

def get_volume():
    sink.handleevents()
    volumes = sink.getvolume()
    retv = 0
    for v in volumes:
        retv += v
    retv /= len(volumes) * 100
    return retv

def publish_volume(v):
    global p
    print("Publishing:", v)
    p.publish(v)

last_v = -1

def change_volume_cb(msg):
    global last_v
    # parse msg
    try:
        v = float(msg)
    except ValueError:
        print("Got illegal value:", msg)
    else:
        print("Setting volume to", v)
        v=int(v*100)
        if v==0:
            sink.setvolume(v)
            sink.setmute(1, alsaaudio.MIXER_CHANNEL_ALL)
        else:
            sink.setmute(0, alsaaudio.MIXER_CHANNEL_ALL)
            sink.setvolume(v)
        publish_volume(v)
        last_v = v
        event_trigger = 50

s = p.subscribe("set", callback=change_volume_cb)

print("MQTT volume controller started.")
print("Controlling: ", sink_name)
print("Listening on: ", location + "/" + device)
print("Press Ctrl-C to stop.")

# main loop
while True:
    v = get_volume()
    if v != last_v:
        last_v = v
        event_trigger = 0        
    event_trigger -= 1
    if event_trigger <= 0:
        publish_volume(v)
        event_trigger = 50 # every 5s
    process()
    sleep(0.1)
