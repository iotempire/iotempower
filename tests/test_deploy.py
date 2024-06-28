import io
import ipaddress
import os
import socket
import time

import paho.mqtt.client as mqtt
import paramiko
import pytest
import sshtunnel
from fabric import Connection
from paramiko.ssh_exception import AuthenticationException

from tests.data import (
    cases_for_deployment,
    default_username,
    gateway_host,
    local_bind_mqtt_port,
    mqtt_listen_period,
    private_key_file_path,
)


def check_for_presence(all_messages, expected_messages):
    return set(expected_messages).issubset(set(all_messages))


def check_ip_presence(all_messages, node_name):
    for first, second in all_messages:
        if first == f"iotempower/_cfg_/{node_name}/ip":
            ipaddress.ip_address(second)
            return True
    raise False


def generate_file(lines):
    return io.BytesIO("\n".join(lines).encode("utf-8"))


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


@pytest.fixture(scope="module")
def ssh_client():
    conn = Connection(host=f"{default_username}@{gateway_host}", connect_timeout=5)
    try:
        conn.run("uname -a")
        yield conn
        conn.close()
    except socket.timeout:
        raise Exception(f"Failed to connect to {gateway_host}")
    except AuthenticationException:
        raise Exception(
            f"Authentication failed for {gateway_host}, "
            f"please make sure you have right configuration, https://www.ssh.com/academy/ssh/copy-id"
        )


@pytest.fixture(scope="module")
def sftp_client(ssh_client):
    transport = paramiko.Transport((gateway_host, 22))
    private_key_file = os.path.expanduser(private_key_file_path)
    pkey = paramiko.RSAKey.from_private_key_file(private_key_file)
    transport.connect(username="iot", pkey=pkey)
    sftp_client = paramiko.SFTPClient.from_transport(transport)
    yield sftp_client
    sftp_client.close()


@pytest.fixture
def new_node(ssh_client):
    ssh_client.run("rm -rf iot-systems/demo01/testing-node")  # TODO Creating your iot-systems folder for testing
    ssh_client.run("cd iot-systems/demo01 && iot x create_node_template testing-node")


@pytest.mark.parametrize("device_name, commands, expected_messages", cases_for_deployment)
def test_deploy(new_node, ssh_client, sftp_client, device_name, commands, expected_messages):
    # TODO compile stuff in the test machine and the copy them over to rasperry pi and then flash it there.
    sftp_client.putfo(generate_file(commands), "iot-systems/demo01/testing-node/setup.cpp")
    ssh_client.run(f"echo 'board=\"{device_name}\"' > iot-systems/demo01/testing-node/node.conf")
    ssh_client.run("cd iot-systems/demo01/testing-node && iot x deploy serial")
    messages = mqtt_read()
    assert check_for_presence(all_messages=messages, expected_messages=expected_messages)
    assert check_ip_presence(all_messages=messages, node_name="testing-node")
