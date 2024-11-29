import os
import socket

import paho.mqtt.client as mqtt
import paramiko
import pytest
import sshtunnel
from fabric import Connection
from paramiko.ssh_exception import AuthenticationException

from tests.conf_data import default_username, gateway_host, local_bind_mqtt_port, private_key_file_path

def pytest_addoption(parser):
    parser.addoption("--boards", action="store", default=None, help="Specify the board(s) to test")
    parser.addoption("--devices", action="store", default=None, help="Specify the device(s) to test")

@pytest.fixture(scope="module", autouse=False)
def ssh_client():
    conn = Connection(host=f"{default_username}@{gateway_host}", connect_timeout=5)
    try:
        conn.run("echo Hello IoTempower Testing")
        yield conn
        conn.close()
    except socket.timeout:
        raise Exception(f"Failed to connect to {gateway_host}")
    except AuthenticationException:
        raise Exception(
            f"Authentication failed for {gateway_host}, "
            f"please make sure you have right configuration, https://www.ssh.com/academy/ssh/copy-id"
        )


@pytest.fixture(scope="module", autouse=False)
def sftp_client(ssh_client):
    transport = paramiko.Transport((gateway_host, 22))
    private_key_file = os.path.expanduser(private_key_file_path)
    pkey = paramiko.RSAKey.from_private_key_file(private_key_file)
    transport.connect(username="iot", pkey=pkey)
    sftp_client = paramiko.SFTPClient.from_transport(transport)
    yield sftp_client
    sftp_client.close()


@pytest.fixture(scope="module", autouse=False)
def mqtt_client():
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    with sshtunnel.open_tunnel(
        gateway_host,
        ssh_username=default_username,
        remote_bind_address=("127.0.0.1", 1883),
        local_bind_address=("127.0.0.1", local_bind_mqtt_port),
    ):
        mqttc.connect("127.0.0.1", local_bind_mqtt_port, 60)
        yield mqttc
        mqttc.disconnect()
