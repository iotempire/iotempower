# Handler for simple lock, based on relay

import gc
import wifi
import machine
from machine import Pin
import time
import ubinascii
from umqtt.simple import MQTTClient

gc.collect()

import config

# make unique
config.mqtt_client_id += b"_" + ubinascii.hexlify(machine.unique_id())

state_topic = config.mqtt_topic
command_topic = state_topic + b"/set"

KEEP_OPEN_TIME = 15  # How long keep lock open if not renewed

OVERFLOW = 1000

led = Pin(2, Pin.OUT)
lock = Pin(5, Pin.OUT)
lock.low()
door_closed = True

lock_open_time = 0


def publish_status():
    global door_closed
    global client

    try:
        if door_closed:
            client.publish(state_topic, b'closed')
        else:
            client.publish(state_topic, b'open')
    except:
        init_client()
    print('Publishing status door locked: {}'.format(door_closed))


def lock_open():
    global door_closed
    global lock_open_time
    lock_open_time = 0  # reset time each time we get a request to open
    print("Opening lock.")
    lock.high()
    door_closed = False
    publish_status()


def lock_close():
    global door_closed
    print("Closing lock.")
    lock.low()
    door_closed = True
    publish_status()


def callback(topic, msg):
    global scene
    msg = msg.decode().lower()  # convert in "real" string
    print("Received in callback:", (topic, msg))
    if msg == 'open':
        lock_open()
    elif msg.startswith('close') or msg.startswith('lock'):
        lock_close()


def init_client():
    global client

    print("Trying to connect to mqtt broker.")
    wifi.connect()
    try:
        client = MQTTClient(config.mqtt_client_id, config.mqtt_broker, user=config.mqtt_user,
                            password=config.mqtt_password)
        client.set_callback(callback)
        client.connect()
        print("Connected to {}".format(config.mqtt_broker))
        client.subscribe(command_topic)
        print("Subscribed to %s topic" % t)
    except:
        print("Trouble to init mqtt.")


def receive_sub():
    global client
    try:
        client.check_msg()  # non blocking
    except:
        print("Trouble to receive from mqtt")


def run():
    global lock_open_time, door_closed

    init_client()
    counter = 0
    last_press = -100

    while True:
        if counter % 10 == 0:  # every 10th of second
            receive_sub()
            # automatically lock open door after 5s
            if not door_closed:
                lock_open_time += 1
                if lock_open_time % 10 == 0:
                    print("Door is open now for this number of s:", lock_open_time / 10)
                if lock_open_time > KEEP_OPEN_TIME * 10:
                    lock_open_time = 0
                    lock_close();

        if counter % 500 == 0:  # every 5s
            publish_status()

        if counter % 100 == 0:  # every second
            if not door_closed:
                if led() == 1:  # blink when open
                    led.low()
                else:
                    led.high()
            else:
                led.low()  # blue light on when open

        time.sleep(0.01)
        counter += 1
        if counter >= OVERFLOW:
            counter = 0
