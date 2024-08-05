import ipaddress

import pytest

from tests.conf_data import (
    cases_for_deployment,
    nodes_folder_path,
)
from tests.utils import check_for_presence, generate_file, mqtt_listen


def check_ip_presence(all_messages, node_name):
    for first, second in all_messages:
        if first == f"iotempower/_cfg_/{node_name}/ip":
            ipaddress.ip_address(second)
            return True
    raise False


@pytest.fixture
def new_node(ssh_client):
    ssh_client.run(f"rm -rf {nodes_folder_path}/testing-node")  # TODO Creating your iot-systems folder for testing
    ssh_client.run(f"cd {nodes_folder_path} && iot x create_node_template testing-node")


@pytest.mark.parametrize("device_name, commands, expected_messages", cases_for_deployment)
def test_deploy(new_node, ssh_client, sftp_client, mqtt_client, device_name, commands, expected_messages):
    # TODO compile stuff in the test machine and the copy them over to rasperry pi and then flash it there.
    sftp_client.putfo(generate_file(lines=commands), f"{nodes_folder_path}/testing-node/setup.cpp")
    ssh_client.run(f"echo 'board=\"{device_name}\"' > {nodes_folder_path}/testing-node/node.conf")
    ssh_client.run(f"cd {nodes_folder_path}/testing-node && iot x deploy serial")

    # Subscribe to node topics for status messages
    for topic, message in expected_messages:
        mqtt_client.subscribe(topic)
    # Subscribe to IP definition topic
    mqtt_client.subscribe(topic="iotempower/_cfg_/testing-node/ip")

    # Listen for status messages of tested node and write them to messages list
    messages = mqtt_listen(mqtt_client)
    assert check_for_presence(all_messages=messages, expected_messages=expected_messages)
    assert check_ip_presence(all_messages=messages, node_name="testing-node")
