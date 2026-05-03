import shutil
import socket
import subprocess
import threading
import time
from pathlib import Path

import paho.mqtt.client as mqtt
import pytest


DEMO_PAYLOAD = "IOTEMPOWER_TLS_DEMO_MESSAGE"
DEMO_TOPIC = "iotempower/demo/wire"


def _find_free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def _run(cmd: list[str], cwd: Path) -> None:
    subprocess.run(cmd, cwd=cwd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


class SniffingProxy:
    """Tiny TCP proxy that forwards traffic and stores raw client->server bytes."""

    def __init__(self, listen_port: int, target_port: int):
        self.listen_port = listen_port
        self.target_port = target_port
        self.client_to_server = bytearray()
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._serve, daemon=True)

    def start(self) -> None:
        self._thread.start()
        # Give the listener a moment to bind before clients connect.
        time.sleep(0.1)

    def stop(self) -> None:
        self._stop.set()
        self._thread.join(timeout=1)

    def _serve(self) -> None:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server.bind(("127.0.0.1", self.listen_port))
            server.listen(1)
            server.settimeout(0.2)
            while not self._stop.is_set():
                try:
                    client_sock, _ = server.accept()
                except socket.timeout:
                    continue
                with client_sock:
                    with socket.create_connection(("127.0.0.1", self.target_port), timeout=5) as upstream:
                        client_sock.settimeout(0.2)
                        upstream.settimeout(0.2)
                        while not self._stop.is_set():
                            moved = False
                            try:
                                data = client_sock.recv(4096)
                                if not data:
                                    break
                                self.client_to_server.extend(data)
                                upstream.sendall(data)
                                moved = True
                            except socket.timeout:
                                pass
                            try:
                                resp = upstream.recv(4096)
                                if not resp:
                                    break
                                client_sock.sendall(resp)
                                moved = True
                            except socket.timeout:
                                pass
                            if not moved:
                                time.sleep(0.01)
                break  # One connection is enough for this demo.


def _mk_client(use_tls: bool, ca_file: Path | None = None) -> mqtt.Client:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    if use_tls:
        assert ca_file is not None
        client.tls_set(ca_certs=str(ca_file))
    return client


def _ascii_preview(data: bytes, max_len: int = 120) -> str:
    cleaned = []
    for b in data[:max_len]:
        if 32 <= b <= 126:
            cleaned.append(chr(b))
        else:
            cleaned.append(".")
    return "".join(cleaned)


@pytest.mark.skipif(
    shutil.which("openssl") is None or shutil.which("mosquitto") is None,
    reason="openssl and mosquitto are required for TLS wire demo",
)
def test_tls_hides_payload_on_wire_demo(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    cert_dir = tmp_path / "certs"
    cert_dir.mkdir(parents=True, exist_ok=True)

    # Build CA + server cert with SAN so modern TLS validation succeeds.
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
            "/CN=IoTempowerWireDemoCA",
        ],
        cert_dir,
    )
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
    (cert_dir / "server.ext").write_text("subjectAltName = IP:127.0.0.1,DNS:localhost\n", encoding="utf-8")
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

    plain_port = _find_free_port()
    tls_port = _find_free_port()
    plain_proxy_port = _find_free_port()
    tls_proxy_port = _find_free_port()

    conf_file = tmp_path / "mosquitto_demo.conf"
    conf_file.write_text(
        "\n".join(
            [
                f"listener {plain_port}",
                "allow_anonymous true",
                f"listener {tls_port}",
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

    plain_proxy = SniffingProxy(plain_proxy_port, plain_port)
    tls_proxy = SniffingProxy(tls_proxy_port, tls_port)
    plain_proxy.start()
    tls_proxy.start()

    try:
        time.sleep(0.5)
        if broker.poll() is not None:
            stdout, stderr = broker.communicate(timeout=2)
            pytest.fail(f"mosquitto failed to start.\nstdout:\n{stdout}\nstderr:\n{stderr}")

        # Plain MQTT publish through sniffing proxy.
        plain_client = _mk_client(use_tls=False)
        plain_client.connect("127.0.0.1", plain_proxy_port, 10)
        plain_client.loop_start()
        plain_client.publish(DEMO_TOPIC, payload=DEMO_PAYLOAD, qos=0, retain=False)
        time.sleep(0.3)
        plain_client.loop_stop()
        plain_client.disconnect()

        # TLS MQTT publish through sniffing proxy.
        tls_client = _mk_client(use_tls=True, ca_file=cert_dir / "ca.crt")
        tls_client.connect("127.0.0.1", tls_proxy_port, 10)
        tls_client.loop_start()
        tls_client.publish(DEMO_TOPIC, payload=DEMO_PAYLOAD, qos=0, retain=False)
        time.sleep(0.5)
        tls_client.loop_stop()
        tls_client.disconnect()

        plain_bytes = bytes(plain_proxy.client_to_server)
        tls_bytes = bytes(tls_proxy.client_to_server)
        marker = DEMO_PAYLOAD.encode("utf-8")

        plain_has_marker = marker in plain_bytes
        tls_has_marker = marker in tls_bytes

        with capsys.disabled():
            print("\n=== MQTT Wire Demo ===")
            print(f"Payload marker: {DEMO_PAYLOAD}")
            print("")
            print("[Plain MQTT]")
            print(f"- Captured bytes: {len(plain_bytes)}")
            print(f"- Payload visible on wire: {plain_has_marker}")
            print(f"- ASCII preview: {_ascii_preview(plain_bytes)}")
            print("")
            print("[TLS MQTT]")
            print(f"- Captured bytes: {len(tls_bytes)}")
            print(f"- Payload visible on wire: {tls_has_marker}")
            print(f"- ASCII preview: {_ascii_preview(tls_bytes)}")
            print("")
            if plain_has_marker and not tls_has_marker:
                print("Result: PASS - plain MQTT exposes payload, TLS hides it.")
            else:
                print("Result: FAIL - expected payload visibility difference not observed.")

        assert plain_has_marker, "Expected plaintext MQTT to expose payload bytes on wire"
        assert not tls_has_marker, "Expected TLS MQTT to hide payload bytes on wire"
    finally:
        plain_proxy.stop()
        tls_proxy.stop()
        broker.terminate()
        try:
            broker.wait(timeout=3)
        except subprocess.TimeoutExpired:
            broker.kill()
