# single led rgb handler
import wifi
import machine
from machine import Pin, PWM
import time
import ubinascii
from umqtt.simple import MQTTClient

import config

# make unique
config.mqtt_client_id += b"_" + ubinascii.hexlify(machine.unique_id())

state_topic = config.mqtt_topic + b"/status"
command_topic = config.mqtt_topic + b"/set"
brightness_state_topic = config.mqtt_topic + b'/brightness/status'
brightness_command_topic = config.mqtt_topic + b'/brightness/set'
rgb_state_topic = config.mqtt_topic + b'/rgb/status'
rgb_command_topic = config.mqtt_topic + b'/rgb/set'

light_is_on = False
brightness = 0
last_rgb = (0, 0, 0)

current_rgb = (0, 0, 0)

green_pin = PWM(Pin(2))
red_pin = PWM(Pin(0))
blue_pin = PWM(Pin(4))

OVERFLOW = 1000


def set_light(r, g, b):
    global np, light_is_on, brightness, last_rgb, current_rgb
    print("Setting light to:", (r, g, b))
    light_is_on = (r + g + b > 0)
    if not light_is_on:  # if turned off, get state to save
        last_rgb = current_rgb
    current_rgb = (r, g, b)
    red_pin.duty(int(r * 1023 / 255))
    green_pin.duty(int(g * 1023 / 255))
    blue_pin.duty(int(b * 1023 / 255))
    brightness = int((r + g + b) / 3)
    publish_status()


def set_brightness(b):
    set_light(b, b, b)


def off():
    set_light(0, 0, 0)


def on():
    global last_rgb
    (r, g, b) = current_rgb
    if r > 0 or g > 0 or b > 0:
        # already on, nothing necessary
        pass
    else:
        if last_rgb == (0, 0, 0):
            set_light(255, 255, 255)  # full brightness
        else:
            (r, g, b) = last_rgb
            set_light(r, g, b)


def publish_status():
    global light_is_on
    global np
    global brightness
    global client

    try:
        if light_is_on:
            client.publish(state_topic, b'on')
        else:
            client.publish(state_topic, b'off')
        client.publish(brightness_state_topic, str(brightness).encode())
        client.publish(rgb_state_topic, ("%d,%d,%d" % current_rgb).encode())
    except:
        init_client()
    print('Publishing status light is on: {}'.format(light_is_on))


def callback(topic, msg):
    if topic == command_topic:
        print("Received in status callback:", msg)
        msg = msg.decode().lower()
        if msg.startswith('on'):
            on()
        elif msg.startswith('off'):
            off()
    elif topic == brightness_command_topic:
        print("Received in brightness callback:", msg)
        try:
            set_brightness(int(msg.decode()))
        except:
            pass
    elif topic == rgb_command_topic:
        print("Received in rgb callback:", msg)
        try:
            set_light(*(map(int, msg.decode().split(","))))
        except:
            pass


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
        t = config.mqtt_topic + b"/#"
        client.subscribe(t)
        print("Subscribed to %s topic" % t)
    #        client.subscribe(command_topic)
    #        client.subscribe(brightness_command_topic)
    #        client.subscribe(rgb_command_topic)
    except:
        print("Trouble to init mqtt.")


def receive_sub():
    global client
    try:
        client.check_msg()  # non blocking
    except:
        print("Trouble to receive from mqtt.")


def run():
    init_client()
    counter = 0
    last_press = -100
    global lock_open_time

    while True:
        if counter % 10 == 0:  # every 10th of second
            receive_sub()

        if counter % 500 == 0:  # every 5s
            publish_status()

        time.sleep(0.01)
        counter += 1
        if counter >= OVERFLOW:
            counter = 0
