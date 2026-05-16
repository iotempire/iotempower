import os
import subprocess
from pathlib import Path

import pytest


def _run(command, cwd, **kwargs):
    return subprocess.run(
        command,
        cwd=cwd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=True,
        **kwargs,
    )


def _generate_certificates(tmp_path: Path, host: str) -> Path:
    system_dir = tmp_path / "tls-system"
    cert_dir = system_dir / "certs"
    system_dir.mkdir()
    (system_dir / "system.conf").write_text(
        f'IOTEMPOWER_MQTT_HOST="{host}"\n'
        f'IOTEMPOWER_MQTT_CERT_FOLDER="{cert_dir}"\n',
        encoding="utf-8",
    )

    env = os.environ.copy()
    if env.get("IOTEMPOWER_ACTIVE") != "yes":
        pytest.skip("IoTempower environment is not active")

    _run(["mqtt_generate_certificates"], cwd=system_dir, env=env)
    return cert_dir


@pytest.mark.parametrize(
    ("host", "verify_arg", "expected_sans"),
    [
        ("192.0.2.10", "-verify_ip", ["IP Address:192.0.2.10", "DNS:192.0.2.10"]),
        ("mqtt-test.local", "-verify_hostname", ["DNS:mqtt-test.local"]),
    ],
)
def test_mqtt_tls_certificates_include_verifiable_san(tmp_path, host, verify_arg, expected_sans):
    cert_dir = _generate_certificates(tmp_path, host)

    cert_text = _run(
        ["openssl", "x509", "-in", "server.crt", "-noout", "-text"],
        cwd=cert_dir,
    ).stdout
    for expected_san in expected_sans:
        assert expected_san in cert_text
    assert "TLS Web Server Authentication" in cert_text
    assert "Digital Signature" in cert_text
    assert "Public Key Algorithm: id-ecPublicKey" in cert_text

    _run(
        [
            "openssl",
            "verify",
            "-CAfile",
            "ca.crt",
            "-purpose",
            "sslserver",
            verify_arg,
            host,
            "server.crt",
        ],
        cwd=cert_dir,
    )
