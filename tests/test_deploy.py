import ipaddress
import time

import paho.mqtt.client as mqtt
import pytest
import sshtunnel

from tests.conf_data import (
    cases_for_deployment,
    default_username,
    gateway_host,
    local_bind_mqtt_port,
    mqtt_listen_period,
    nodes_folder_path,
)
from tests.utils import check_for_presence, generate_file


def check_ip_presence(all_messages, node_name):
    for first, second in all_messages:
        if first == f"iotempower/_cfg_/{node_name}/ip":
            ipaddress.ip_address(second)
            return True
    raise False


# Tried to export mqtt client as a function however sshtunnel made an exception while creating ssh connection
# TODO try to refactor this since MQTT client is used for both
def mqtt_read() -> list[tuple[str, str]]:
    messages: list[tuple[str, str]] = []

    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, reason_code, properties):
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # TODO consider just listening to the configuration and node topic
        client.subscribe("#")
        print("Connected to MQTT broker and subscribed to topic #")

    # The callback for when a PUBLISH message is received from the server.
    def on_message(client, userdata, msg):
        messages.append((msg.topic, msg.payload.decode("utf-8")))

    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    with sshtunnel.open_tunnel(
        gateway_host,
        ssh_username=default_username,
        remote_bind_address=("127.0.0.1", 1883),
        local_bind_address=("127.0.0.1", local_bind_mqtt_port),
    ):
        mqttc.connect("127.0.0.1", local_bind_mqtt_port, 60)
        # creates a loop in a new thread
        mqttc.loop_start()
        time.sleep(mqtt_listen_period)
        mqttc.loop_stop()
        mqttc.disconnect()
    return messages


@pytest.fixture
def new_node(ssh_client):
    ssh_client.run(f"rm -rf {nodes_folder_path}/testing-node")  # TODO Creating your iot-systems folder for testing
    ssh_client.run(f"cd {nodes_folder_path} && iot x create_node_template testing-node")


@pytest.mark.parametrize("device_name, commands, expected_messages", cases_for_deployment)
def test_deploy(new_node, ssh_client, sftp_client, device_name, commands, expected_messages):
    # TODO compile stuff in the test machine and the copy them over to rasperry pi and then flash it there.
    sftp_client.putfo(generate_file(lines=commands), f"{nodes_folder_path}/testing-node/setup.cpp")
    ssh_client.run(f"echo 'board=\"{device_name}\"' > {nodes_folder_path}/testing-node/node.conf")
    ssh_client.run(f"cd {nodes_folder_path}/testing-node && iot x deploy serial")
    messages = mqtt_read()
    assert check_for_presence(all_messages=messages, expected_messages=expected_messages)
    assert check_ip_presence(all_messages=messages, node_name="testing-node")
