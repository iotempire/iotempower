import time

import pytest

from tests.conf_data import (
    cases_for_hardware,
    nodes_folder_path,
    tested_device_address,
    tested_node_name,
    tester_device_address,
    tester_node_name,
)
from tests.utils import check_for_presence, generate_file, mqtt_listen


@pytest.fixture
def new_nodes(ssh_client):
    ssh_client.run(f"rm -rf {nodes_folder_path}/hardware-testing")  # TODO Creating your iot-systems folder for testing
    ssh_client.run(f"mkdir {nodes_folder_path}/hardware-testing")
    ssh_client.run(f"cd {nodes_folder_path}/hardware-testing && iot x create_node_template {tested_node_name}")
    ssh_client.run(f"cd {nodes_folder_path}/hardware-testing && iot x create_node_template {tester_node_name}")


@pytest.mark.parametrize("device_names, example_syntaxes, mqtt_messages", cases_for_hardware)
def test_hardware(mqtt_client, new_nodes, ssh_client, sftp_client, device_names, example_syntaxes, mqtt_messages):
    # Extracting configurations from tuple objects
    tested_device_name, tester_device_name = device_names
    example_syntax_tested, example_syntax_tester = example_syntaxes
    initial_tested_status, tester_messages_to_send, tested_messages_to_receive = mqtt_messages

    tested_node_path = f"{nodes_folder_path}/hardware-testing/{tested_node_name}"
    tester_node_path = f"{nodes_folder_path}/hardware-testing/{tester_node_name}"

    # Assigning device names of tested and tester nodes
    ssh_client.run(f"echo 'board=\"{tested_device_name}\"' > {tested_node_path}/node.conf")
    ssh_client.run(f"echo 'board=\"{tester_device_name}\"' > {tester_node_path}/node.conf")

    sftp_client.putfo(generate_file(lines=example_syntax_tested), f"{tested_node_path}/setup.cpp")
    sftp_client.putfo(generate_file(lines=example_syntax_tester), f"{tester_node_path}/setup.cpp")

    # Deployment
    ssh_client.run(f"cd {tested_node_path} && iot x deploy serial {tested_device_address}")
    time.sleep(3)
    ssh_client.run(f"cd {tester_node_path} && iot x deploy serial {tester_device_address}")

    # Subscribe to tested node topic for initial status message
    for topic, message in initial_tested_status:
        mqtt_client.subscribe(topic)
    # Listen for initial status message of tested node and write them to messages list
    messages = mqtt_listen(mqtt_client)
    assert check_for_presence(all_messages=messages, expected_messages=initial_tested_status)

    # Sending command through MQTT
    for topic, payload in tester_messages_to_send:
        mqtt_client.publish(topic, payload)

    # Listening for expected status message of tested node after command
    # Subscription to the topics and on_message functionality was written in upper lines
    messages = mqtt_listen(mqtt_client)
    assert check_for_presence(all_messages=messages, expected_messages=tested_messages_to_receive)
