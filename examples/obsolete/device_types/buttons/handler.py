# Handler for ulno-iot devkit1
import config

import wifi
import machine
from machine import Pin
import time
import ubinascii
from umqtt.simple import MQTTClient

# make unique
config.mqtt_client_id += b"_" + ubinascii.hexlify(machine.unique_id())

button_count = len(config.button_pins)

button_pins=[]
button_topic=[]
count=1
for bp in config.button_pins:
    button_pins.append( Pin(bp,Pin.IN,Pin.PULL_UP) )
    button_topic.append( config.mqtt_topic + b"/" + str(count).encode() )
    count += 1


OVERFLOW = 1000

onoff = [b'off',b'on']

def publish_status():
    global client
    
    try:
        for i in range(button_count):
            client.publish( button_topic[i], onoff[button_pins[i]()] )
        print('Published status.')
    except:
        print('Trouble publishing.')
        init_client()


def init_client():
    global client
    
    print( "Trying to connect to mqtt broker." )
    wifi.connect()
    try:
        client = MQTTClient(config.mqtt_client_id, config.mqtt_broker, user=config.mqtt_user, password=config.mqtt_password)
        client.connect()
        print("Connected to {}".format(config.mqtt_broker))
        t = config.mqtt_topic + b"/#"
        client.subscribe(t)
        print("Subscribed to %s topic" % t)
    except:
        print( "Trouble to init mqtt." )


        
def run():
    init_client()
    counter = 0
    last_state = [1] * button_count
    
    while True:
        if counter % 10 == 0: # every 10th of second
            changed = False
            for i in range(button_count):
                if last_state[i] != button_pins[i]():
                    changed = True
                last_state[i] = button_pins[i]()
            if changed:
                publish_status()
            
        if counter%500 == 0: # every 5s
            publish_status()
        
        time.sleep(0.01)
        counter += 1
        if counter >= OVERFLOW:
            counter = 0
