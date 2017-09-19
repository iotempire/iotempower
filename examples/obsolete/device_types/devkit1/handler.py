# Handler for ulno-iot devkit1
import config

if config.display:
    import ulno_iot_display as dp
if config.devel:
    import ulno_iot_devel as dv
if config.ht:
    import ulno_iot_ht as ht
import gc
gc.collect()

import wifi
import machine
from machine import Pin
import time
import ubinascii
from umqtt.simple import MQTTClient
gc.collect()

# make unique
config.mqtt_client_id += b"_" + ubinascii.hexlify(machine.unique_id())

blue_state_topic = config.mqtt_topic + b"/blue"
blue_command_topic = blue_state_topic + b"/set"
if config.devel:
    red_state_topic = config.mqtt_topic + b"/red"
    red_command_topic = red_state_topic + b"/set"
    yellow_state_topic = config.mqtt_topic + b"/yellow"
    yellow_command_topic = yellow_state_topic + b"/set"
    left_state_topic = config.mqtt_topic + b"/left"
    right_state_topic = config.mqtt_topic + b"/right"
    bottom_state_topic = config.mqtt_topic + b"/bottom"
if config.ht:
    temperature_state_topic = config.mqtt_topic + b"/temperature"
    humidity_state_topic = config.mqtt_topic + b"/humidity"
if config.display:
    text_command_topic = config.mqtt_topic + b"/text"


OVERFLOW = 1000

onoff = [b'off',b'on']

def publish_status():
    global client
    
    try:
        client.publish( blue_state_topic, onoff[dv.blue()^1] )
        if config.devel:
            client.publish( red_state_topic, onoff[dv.red()] )
            client.publish( yellow_state_topic, onoff[dv.yellow()] )
            client.publish( left_state_topic, onoff[dv.left_button()^1] )
            client.publish( right_state_topic, onoff[dv.right_button()^1] )
            client.publish( bottom_state_topic, onoff[dv.lower_button()^1] )
        if config.ht:
            client.publish( temperature_state_topic, str(ht.temperature()).encode() )
            client.publish( humidity_state_topic, str(ht.humidity()).encode() )
        print('Published status.')
    except:
        print('Trouble publishing.')
        init_client()


def callback(topic, msg):
    if config.devel and topic == red_command_topic:
      print("Received red in callback:", msg)
      msg = msg.decode().lower()
      if msg.startswith( 'on' ):
        dv.red.high()
      elif msg.startswith( 'off' ):
        dv.red.low()
    elif config.devel and topic == yellow_command_topic:
      print("Received yellow in callback:", msg)
      msg = msg.decode().lower()
      if msg.startswith( 'on' ):
        dv.yellow.high()
      elif msg.startswith( 'off' ):
        dv.yellow.low()
    elif config.devel and topic == blue_command_topic:
      msg = msg.decode().lower()
      if msg.startswith( 'on' ):
        dv.blue.low()
      elif msg.startswith( 'off' ):
        dv.blue.high()
    elif config.display and topic == text_command_topic:
      print("Received text in callback:", msg)
      try:
        msg = msg.decode()
        if msg == "&&clear":
          dp.clear()
        else:
          dp.println( msg )
      except:
        pass
        

def init_client():
    global client
    
    print( "Trying to connect to mqtt broker." )
    wifi.connect()
    try:
        client = MQTTClient(config.mqtt_client_id, config.mqtt_broker, user=config.mqtt_user, password=config.mqtt_password)
        client.set_callback(callback)
        client.connect()
        print("Connected to {}".format(config.mqtt_broker))
        t = config.mqtt_topic + b"/#"
        client.subscribe(t)
        print("Subscribed to %s topic" % t)
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
    last_left = dv.left_button()
    last_right = dv.right_button()
    last_lower = dv.lower_button()
    
    while True:
        if counter % 10 == 0: # every 10th of second
            receive_sub()
            if last_left != dv.left_button() \
                or last_right != dv.right_button() \
                or last_lower != dv.lower_button():
                    last_left = dv.left_button()
                    last_right = dv.right_button()
                    last_lower = dv.lower_button()
                    publish_status()
            
        if counter%500 == 0: # every 5s
            publish_status()
        
        time.sleep(0.01)
        counter += 1
        if counter >= OVERFLOW:
            counter = 0
