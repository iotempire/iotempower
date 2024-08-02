import os
import socket

import paramiko
import pytest
from fabric import Connection
from paramiko.ssh_exception import AuthenticationException

from tests.conf_data import default_username, gateway_host, private_key_file_path


@pytest.fixture(scope="module", autouse=False)
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


@pytest.fixture(scope="module", autouse=False)
def sftp_client(ssh_client):
    transport = paramiko.Transport((gateway_host, 22))
    private_key_file = os.path.expanduser(private_key_file_path)
    pkey = paramiko.RSAKey.from_private_key_file(private_key_file)
    transport.connect(username="iot", pkey=pkey)
    sftp_client = paramiko.SFTPClient.from_transport(transport)
    yield sftp_client
    sftp_client.close()
