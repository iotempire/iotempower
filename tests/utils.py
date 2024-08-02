import io
import time
from typing import Generator

import paho.mqtt.client as mqtt
import sshtunnel

from tests.conf_data import (
    default_username,
    gateway_host,
    local_bind_mqtt_port,
    mqtt_listen_period,
)


def check_for_presence(all_messages, expected_messages):
    return set(expected_messages).issubset(set(all_messages))


def generate_file(lines):
    return io.BytesIO("\n".join(lines).encode("utf-8"))


# This implementation somehow doesn't work, ssh tunnel raises exception
def get_mqtt_client() -> Generator[mqtt.Client, None, None]:
    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, reason_code, properties):
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # TODO consider just listening to the configuration and node topic
        client.subscribe("#")
        print("Connected to MQTT broker and subscribed to topic #")

    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    with sshtunnel.open_tunnel(
        gateway_host,
        ssh_username=default_username,
        remote_bind_address=("127.0.0.1", 1883),
        local_bind_address=("127.0.0.1", local_bind_mqtt_port),
        debug_level="DEBUG",
    ):
        print("Connected to MQTT broker and subscribed to topic #")
        mqttc.connect("127.0.0.1", local_bind_mqtt_port, 60)
        yield mqttc
        mqttc.disconnect()


def mqtt_read():
    messages: list[tuple[str, str]] = []

    def on_message(client, userdata, msg):
        messages.append((msg.topic, msg.payload.decode("utf-8")))

    client = next(get_mqtt_client())
    client.on_message = on_message

    client.loop_start()
    time.sleep(mqtt_listen_period)
    client.loop_stop()
