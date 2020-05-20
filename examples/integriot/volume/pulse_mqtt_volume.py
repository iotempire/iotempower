mqtt_server = "homeaut"
sink_name = 'alsa_output.usb-Altec_Lansing_Technologies__Inc._Altec_Lansing_XT2_-_USB_Audio-00.analog-stereo'
location = "kitchen"
device = "volume"

from pulsectl import Pulse # we need this here: https://pypi.org/project/pulsectl/
from integriot import *
from time import sleep

# pulse mixer setup
pulse = Pulse("pulse_mqtt_volume")
sinkoutput = pulse.get_sink_by_name(sink_name)

# mqtt setup
init(mqtt_server)
prefix(location)
p = publisher(device)

def get_volume():
    # needs to be looked up each time as we don't get a volume else
    sinkinfo = pulse.sink_info(sinkoutput.index)
    return sinkinfo.volume.value_flat

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
        print("Got illegal value:",msg)
    else:
        print("Setting volume to", v)
        pulse.volume_set_all_chans(sinkoutput, v)
        publish_volume(v)        

s = p.subscribe("set", callback=change_volume_cb)

print("MQTT volume controller started.")
print("Controlling: ", sinkoutput.description)
print("Listening on: ", location + "/" + device)
print("Press Ctrl-C to stop.")
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
