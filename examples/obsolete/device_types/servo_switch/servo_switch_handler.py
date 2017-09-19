# servo light switch control
import wifi
import machine
from machine import Pin
import time
import ubinascii
from umqtt.simple import MQTTClient
import servo_switch

import config
# make unique
config.mqtt_client_id += b"_" + ubinascii.hexlify(machine.unique_id())

topic = config.mqtt_topic
topic_set = topic+"/set"

light_is_on = False

OVERFLOW = 1000

led = Pin(2, Pin.OUT)

def publish_status():
    global light_is_on
    global client
    
    try:
        if light_is_on:
            client.publish(topic, b'on')
        else:
            client.publish(topic, b'off')
    except:
        init_client()
    print('Publishing status light is on: {}'.format(light_is_on))

def on():
    global light_is_on
    print("Turning light on.")
    servo_switch.on()
    light_is_on = True
    publish_status()

def off():
    global light_is_on
    print("Turning light off.")
    servo_switch.off()
    light_is_on = False
    publish_status()

def set_callback(topic, msg):
    global scene
    print("Received in set callback:", (topic, msg))
    msg = msg.decode().lower() # convert in "real" string
    print("Converted in set callback:", (topic, msg))
    if msg.startswith( 'on' ):
        on()
    elif msg.startswith( 'off' ):
        off()
        

def init_client():
    global client
    
    print( "Trying to connect to mqtt broker." )
    wifi.connect()
    try:
        client = MQTTClient(config.mqtt_client_id, config.mqtt_broker, user=config.mqtt_user, password=config.mqtt_password)
        client.set_callback(set_callback)
        client.connect()
        print("Connected to {}".format(config.mqtt_broker))
        client.subscribe(topic_set)
        print("Subscribed to %s topic" % topic_set)
    except:
        print( "Trouble to init mqtt." )

def receive_sub():
    global client
    try:
        client.check_msg() # non blocking
    except:
        print( "Trouble to receive from mqtt." )
        
def run():
    init_client()
    counter = 0
    last_press = -100
    global lock_open_time
    
    while True:
        if counter % 10 == 0: # every 10th of second
            receive_sub()
            
        if counter%500 == 0: # every 5s
            publish_status()
        
        if counter%100 == 0: # every second update light
            if light_is_on:
                led.low()
            else:
                led.high()
                
        time.sleep(0.01)
        counter += 1
        if counter >= OVERFLOW:
            counter = 0
