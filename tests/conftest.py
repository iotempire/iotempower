import os
import socket

import paho.mqtt.client as mqtt
import paramiko
import pytest
import sshtunnel
from fabric import Connection
from paramiko.ssh_exception import AuthenticationException

from tests.conf_data import default_username, gateway_host, isolated_combinations_to_test, local_bind_mqtt_port, private_key_file_path

def pytest_addoption(parser):
    parser.addoption("--boards", action="store", default=None, help="Specify the board(s) to test")
    parser.addoption("--devices", action="store", default=None, help="Specify the device(s) to test")

def pytest_generate_tests(metafunc):
    if "parametrize_board_device" in metafunc.fixturenames:
        boards = metafunc.config.getoption("--boards")
        devices = metafunc.config.getoption("--devices")
        combinations = isolated_combinations_to_test
        if boards:
            board_set = set(boards.split(","))
            combinations = [(b, d, s) for b, d, s in combinations if b in board_set]
        if devices:
            device_set = set(devices.split(","))
            combinations = [(b, d, s) for b, d, s in combinations if d in device_set]
        metafunc.parametrize(
            "parametrize_board_device",
            combinations,
            ids=[f"{b}/{d}" for b, d, s in combinations],
        )


_compilation_results: list[tuple[str, str, str]] = []


def pytest_runtest_logreport(report):
    if report.when == "call" and "test_compilation_isolated" in report.nodeid:
        nodeid = report.nodeid
        if "[" in nodeid:
            param_part = nodeid[nodeid.rfind("[") + 1 : nodeid.rfind("]")]
            if "/" in param_part:
                board, device = param_part.split("/", 1)
                if report.passed:
                    outcome = "PASSED"
                elif report.failed:
                    outcome = "FAILED"
                else:
                    outcome = "ERROR"
                _compilation_results.append((board, device, outcome))


def pytest_terminal_summary(terminalreporter, exitstatus, config):
    if not _compilation_results:
        return
    terminalreporter.write_sep("=", "Compilation Test Summary")
    board_width = max(len("Board"), max(len(b) for b, d, o in _compilation_results))
    device_width = max(len("Device"), max(len(d) for b, d, o in _compilation_results))
    status_width = max(len("Status"), max(len(o) for b, d, o in _compilation_results))
    header = f"{'Board':<{board_width}}  {'Device':<{device_width}}  Status"
    sep = f"{'-' * board_width}  {'-' * device_width}  {'-' * status_width}"
    terminalreporter.write_line(header)
    terminalreporter.write_line(sep)
    for board, device, outcome in _compilation_results:
        line = f"{board:<{board_width}}  {device:<{device_width}}  {outcome}"
        if outcome == "PASSED":
            terminalreporter.write_line(line, green=True)
        else:
            terminalreporter.write_line(line, red=True)

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
