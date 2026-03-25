import shutil
import socket
import subprocess
import time
from pathlib import Path

import paho.mqtt.client as mqtt
import pytest


def _find_free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def _run(cmd: list[str], cwd: Path) -> None:
    subprocess.run(cmd, cwd=cwd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


@pytest.mark.skipif(
    shutil.which("openssl") is None or shutil.which("mosquitto") is None,
    reason="openssl and mosquitto are required for TLS smoke test",
)
def test_local_mqtt_tls_smoke(tmp_path: Path) -> None:
    cert_dir = tmp_path / "certs"
    cert_dir.mkdir(parents=True, exist_ok=True)

    # Create CA certificate.
    _run(
        [
            "openssl",
            "req",
            "-x509",
            "-newkey",
            "rsa:2048",
            "-nodes",
            "-sha256",
            "-days",
            "1",
            "-keyout",
            "ca.key",
            "-out",
            "ca.crt",
            "-subj",
            "/CN=IoTempowerTestCA",
        ],
        cert_dir,
    )

    # Create server key/csr and sign with CA.
    _run(
        [
            "openssl",
            "req",
            "-new",
            "-newkey",
            "rsa:2048",
            "-nodes",
            "-keyout",
            "server.key",
            "-out",
            "server.csr",
            "-subj",
            "/CN=127.0.0.1",
        ],
        cert_dir,
    )
    san_file = cert_dir / "server.ext"
    san_file.write_text("subjectAltName = IP:127.0.0.1,DNS:localhost\n", encoding="utf-8")
    _run(
        [
            "openssl",
            "x509",
            "-req",
            "-in",
            "server.csr",
            "-CA",
            "ca.crt",
            "-CAkey",
            "ca.key",
            "-CAcreateserial",
            "-out",
            "server.crt",
            "-days",
            "1",
            "-sha256",
            "-extfile",
            "server.ext",
        ],
        cert_dir,
    )

    port = _find_free_port()
    conf_file = tmp_path / "mosquitto_tls.conf"
    conf_file.write_text(
        "\n".join(
            [
                f"listener {port}",
                "allow_anonymous true",
                f"cafile {cert_dir / 'ca.crt'}",
                f"certfile {cert_dir / 'server.crt'}",
                f"keyfile {cert_dir / 'server.key'}",
                "require_certificate false",
            ]
        )
        + "\n",
        encoding="utf-8",
    )

    broker = subprocess.Popen(
        ["mosquitto", "-c", str(conf_file)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    try:
        time.sleep(0.5)
        if broker.poll() is not None:
            stdout, stderr = broker.communicate(timeout=2)
            pytest.fail(f"mosquitto failed to start.\nstdout:\n{stdout}\nstderr:\n{stderr}")

        received: list[str] = []
        topic = "iotempower/test/tls"
        payload = "tls-ok"

        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        client.tls_set(ca_certs=str(cert_dir / "ca.crt"))
        client.on_message = lambda _c, _u, msg: received.append(msg.payload.decode("utf-8"))
        client.connect("127.0.0.1", port, 10)
        client.loop_start()
        client.subscribe(topic, qos=0)
        client.publish(topic, payload=payload, qos=0, retain=False)

        deadline = time.time() + 5
        while payload not in received and time.time() < deadline:
            time.sleep(0.05)

        client.loop_stop()
        client.disconnect()

        assert payload in received, "Did not receive MQTT message over TLS"
    finally:
        broker.terminate()
        try:
            broker.wait(timeout=3)
        except subprocess.TimeoutExpired:
            broker.kill()
