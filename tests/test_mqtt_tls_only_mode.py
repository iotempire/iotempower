import os
import subprocess
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]


def _write_executable(path: Path, content: str) -> None:
    path.write_text(content, encoding="utf-8")
    path.chmod(0o755)


def _system_env(tmp_path: Path, fakebin: Path) -> tuple[Path, Path, dict[str, str]]:
    system_dir = tmp_path / "tls-system"
    cert_dir = system_dir / "certs"
    local_dir = tmp_path / "local"
    cert_dir.mkdir(parents=True)
    local_dir.mkdir()
    (cert_dir / "ca.crt").write_text("test-ca\n", encoding="utf-8")
    (cert_dir / "server.crt").write_text("test-server-cert\n", encoding="utf-8")
    (cert_dir / "server.key").write_text("test-server-key\n", encoding="utf-8")
    (system_dir / "system.conf").write_text(
        f'IOTEMPOWER_MQTT_HOST="192.0.2.10"\n'
        'IOTEMPOWER_MQTT_USE_TLS=1\n'
        f'IOTEMPOWER_MQTT_CERT_FOLDER="{cert_dir}"\n',
        encoding="utf-8",
    )

    env = os.environ.copy()
    env.update(
        {
            "IOTEMPOWER_ACTIVE": "yes",
            "IOTEMPOWER_ROOT": str(REPO_ROOT),
            "IOTEMPOWER_LOCAL": str(local_dir),
            "PATH": f"{fakebin}:{REPO_ROOT / 'bin'}:{env['PATH']}",
        }
    )
    return system_dir, cert_dir, env


def _write_fake_mqtt_clients(fakebin: Path) -> Path:
    args_file = fakebin / "mqtt.args"
    _write_executable(
        fakebin / "mosquitto_pub",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_MQTT_ARGS"\n',
    )
    _write_executable(
        fakebin / "mosquitto_sub",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_MQTT_ARGS"\n'
        '[[ "$FAKE_MQTT_OUTPUT" ]] && printf "%s\\n" "$FAKE_MQTT_OUTPUT"\n'
        "exit 0\n",
    )
    return args_file


def _assert_uses_tls_port(args_file: Path, cert_dir: Path) -> None:
    args = args_file.read_text(encoding="utf-8").splitlines()
    assert "-p" in args
    assert args[args.index("-p") + 1] == "8883"
    assert "--cafile" in args
    assert args[args.index("--cafile") + 1] == str(cert_dir / "ca.crt")
    assert "1883" not in args


def test_mqtt_broker_tls_mode_has_no_plaintext_listeners(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _write_executable(fakebin / "pkill", "#!/usr/bin/env bash\nexit 0\n")
    _write_executable(fakebin / "mosquitto", "#!/usr/bin/env bash\nsleep 0.1\nexit 0\n")
    system_dir, _, env = _system_env(tmp_path, fakebin)

    result = subprocess.run(
        ["timeout", "1", "mqtt_broker", "eth0", "192.0.2.10"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )

    assert result.returncode in {0, 124}
    conf = tmp_path / "local" / "tmp" / "mosquitto" / "mosquitto.conf"
    conf_text = conf.read_text(encoding="utf-8")
    assert "listener 8883 192.0.2.10" in conf_text
    assert "listener 1883" not in conf_text


def test_tls_helpers_use_8883_and_cafile(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    args_file = _write_fake_mqtt_clients(fakebin)
    system_dir, cert_dir, env = _system_env(tmp_path, fakebin)
    env["FAKE_MQTT_ARGS"] = str(args_file)

    subprocess.run(["mqtt_send", "/audit/topic", "hello"], cwd=system_dir, env=env, check=True)
    _assert_uses_tls_port(args_file, cert_dir)

    subprocess.run(["mqtt_listen", "/audit/topic"], cwd=system_dir, env=env, check=True)
    _assert_uses_tls_port(args_file, cert_dir)

    env["FAKE_MQTT_OUTPUT"] = "on"
    subprocess.run(
        ["mqtt_action", "/audit/topic", "payload", "on", "true"],
        cwd=system_dir,
        env=env,
        check=True,
    )
    _assert_uses_tls_port(args_file, cert_dir)


def test_get_ips_uses_tls_port_for_discovery(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    args_file = _write_fake_mqtt_clients(fakebin)
    system_dir, cert_dir, env = _system_env(tmp_path, fakebin)
    env["FAKE_MQTT_ARGS"] = str(args_file)
    env["FAKE_MQTT_OUTPUT"] = "iotempower/_cfg_/test-node/ip 192.0.2.55"

    result = subprocess.run(
        ["get_ips", "test-node"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
    )

    assert result.stdout.strip() == "192.0.2.55"
    _assert_uses_tls_port(args_file, cert_dir)
