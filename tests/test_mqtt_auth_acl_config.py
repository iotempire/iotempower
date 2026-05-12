import os
import subprocess
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]


def _write_executable(path: Path, content: str) -> None:
    path.write_text(content, encoding="utf-8")
    path.chmod(0o755)


def _auth_system(tmp_path: Path, fakebin: Path, *, tls: bool = True) -> tuple[Path, Path, dict[str, str]]:
    system_dir = tmp_path / "auth-system"
    cert_dir = system_dir / "certs"
    local_dir = tmp_path / "local"
    node_dir = system_dir / "allowed-node"
    cert_dir.mkdir(parents=True)
    local_dir.mkdir()
    node_dir.mkdir()
    (cert_dir / "ca.crt").write_text("test-ca\n", encoding="utf-8")
    (cert_dir / "server.crt").write_text("test-server-cert\n", encoding="utf-8")
    (cert_dir / "server.key").write_text("test-server-key\n", encoding="utf-8")
    (node_dir / "node.conf").write_text('board="wemos d1 mini"\n', encoding="utf-8")
    (system_dir / "system.conf").write_text(
        f'IOTEMPOWER_MQTT_HOST="127.0.0.1"\n'
        f"IOTEMPOWER_MQTT_USE_TLS={1 if tls else 0}\n"
        f'IOTEMPOWER_MQTT_CERT_FOLDER="{cert_dir}"\n'
        'IOTEMPOWER_MQTT_USER="homeassistant"\n'
        'IOTEMPOWER_MQTT_PW="secretpw"\n'
        'IOTEMPOWER_MQTT_DISCOVERY_PREFIX="iotempower"\n'
        'IOTEMPOWER_MQTT_ACL_EXTRA_WRITE_TOPICS="custom/topic/#"\n',
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


def _fake_broker_tools(fakebin: Path) -> None:
    _write_executable(fakebin / "pkill", "#!/usr/bin/env bash\nexit 0\n")
    _write_executable(
        fakebin / "mosquitto",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_MOSQUITTO_ARGS"\nsleep 0.1\nexit 0\n',
    )
    _write_executable(
        fakebin / "mosquitto_passwd",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_PASSWD_ARGS"\n'
        'if [[ "$1" == "-c" ]]; then read -r password1; read -r password2; '
        '[[ "$password1" == "$password2" ]] || exit 2; '
        'printf "%s:hashed:%s\\n" "$3" "$password1" > "$2"; fi\n'
        "exit 0\n",
    )


def _fake_mqtt_clients(fakebin: Path) -> Path:
    args_file = fakebin / "mqtt.args"
    _write_executable(
        fakebin / "mosquitto_pub",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_MQTT_ARGS"\nexit 0\n',
    )
    _write_executable(
        fakebin / "mosquitto_sub",
        '#!/usr/bin/env bash\nprintf "%s\\n" "$@" > "$FAKE_MQTT_ARGS"\n'
        '[[ "$FAKE_MQTT_OUTPUT" ]] && printf "%s\\n" "$FAKE_MQTT_OUTPUT"\n'
        "exit 0\n",
    )
    return args_file


def _assert_client_uses_tls_auth(args_file: Path, cert_dir: Path) -> None:
    args = args_file.read_text(encoding="utf-8").splitlines()
    assert args[args.index("-p") + 1] == "8883"
    assert args[args.index("--cafile") + 1] == str(cert_dir / "ca.crt")
    assert args[args.index("-u") + 1] == "homeassistant"
    assert args[args.index("-P") + 1] == "secretpw"
    assert "1883" not in args


def test_mqtt_broker_auth_generates_password_acl_and_tls_only_config(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, _, env = _auth_system(tmp_path, fakebin)
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    result = subprocess.run(
        ["timeout", "1", "mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )

    assert result.returncode in {0, 124}
    mosquitto_dir = tmp_path / "local" / "tmp" / "mosquitto"
    conf = (mosquitto_dir / "mosquitto.conf").read_text(encoding="utf-8")
    acl = (mosquitto_dir / "acl_file").read_text(encoding="utf-8")
    password_file = (mosquitto_dir / "password_file").read_text(encoding="utf-8")
    passwd_args = (tmp_path / "passwd.args").read_text(encoding="utf-8").splitlines()

    assert "allow_anonymous false" in conf
    assert f"password_file {mosquitto_dir / 'password_file'}" in conf
    assert f"acl_file {mosquitto_dir / 'acl_file'}" in conf
    assert "listener 8883 127.0.0.1" in conf
    assert "listener 1883" not in conf
    assert "user homeassistant" in acl
    assert "topic read #" in acl
    assert "topic write iotempower/_cfg_/#" in acl
    assert "topic write iotempower/#" in acl
    assert "topic write allowed-node/#" in acl
    assert "topic write custom/topic/#" in acl
    assert password_file == "homeassistant:hashed:secretpw\n"
    assert passwd_args == ["-c", str(mosquitto_dir / "password_file.tmp"), "homeassistant"]
    assert "secretpw" not in passwd_args


def test_mqtt_broker_acl_generation_does_not_source_node_conf(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, _, env = _auth_system(tmp_path, fakebin)
    marker = tmp_path / "node_conf_marker"
    (system_dir / "allowed-node" / "node.conf").write_text(
        f'topic="$(touch${{IFS}}{marker})"\n',
        encoding="utf-8",
    )
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    result = subprocess.run(
        ["timeout", "1", "mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )

    assert result.returncode in {0, 124}
    assert not marker.exists()
    acl = (tmp_path / "local" / "tmp" / "mosquitto" / "acl_file").read_text(encoding="utf-8")
    assert f"topic write $(touch${{IFS}}{marker})/#" in acl


def test_mqtt_broker_acl_honors_simple_custom_node_topic_without_sourcing(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, _, env = _auth_system(tmp_path, fakebin)
    (system_dir / "allowed-node" / "node.conf").write_text(
        'topic="custom/node/topic"\n',
        encoding="utf-8",
    )
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    result = subprocess.run(
        ["timeout", "1", "mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )

    assert result.returncode in {0, 124}
    acl = (tmp_path / "local" / "tmp" / "mosquitto" / "acl_file").read_text(encoding="utf-8")
    assert "topic write custom/node/topic/#" in acl
    assert "topic write allowed-node/#" not in acl


def test_mqtt_broker_acl_accepts_quoted_topic_with_trailing_comment(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, _, env = _auth_system(tmp_path, fakebin)
    (system_dir / "allowed-node" / "node.conf").write_text(
        'topic="custom/topic" # comment\n',
        encoding="utf-8",
    )
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    result = subprocess.run(
        ["timeout", "1", "mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )

    assert result.returncode in {0, 124}
    acl = (tmp_path / "local" / "tmp" / "mosquitto" / "acl_file").read_text(encoding="utf-8")
    assert "topic write custom/topic/#" in acl


def test_mqtt_auth_rejects_password_without_user(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, cert_dir, env = _auth_system(tmp_path, fakebin)
    (system_dir / "system.conf").write_text(
        'IOTEMPOWER_MQTT_HOST="127.0.0.1"\n'
        'IOTEMPOWER_MQTT_USE_TLS=1\n'
        f'IOTEMPOWER_MQTT_CERT_FOLDER="{cert_dir}"\n'
        'IOTEMPOWER_MQTT_PW="password-without-user"\n',
        encoding="utf-8",
    )
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    broker_result = subprocess.run(
        ["mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert broker_result.returncode != 0
    assert "MQTT user and password must both be set" in broker_result.stderr

    args_file = _fake_mqtt_clients(fakebin)
    env["FAKE_MQTT_ARGS"] = str(args_file)
    helper_result = subprocess.run(
        ["mqtt_send", "/allowed-node/probe", "hello"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert helper_result.returncode != 0
    assert "MQTT user and password must both be set" in helper_result.stderr


def test_mqtt_auth_requires_tls(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    _fake_broker_tools(fakebin)
    system_dir, _, env = _auth_system(tmp_path, fakebin, tls=False)
    env["FAKE_MOSQUITTO_ARGS"] = str(tmp_path / "mosquitto.args")
    env["FAKE_PASSWD_ARGS"] = str(tmp_path / "passwd.args")

    result = subprocess.run(
        ["mqtt_broker", "lo", "127.0.0.1"],
        cwd=system_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    assert result.returncode != 0
    assert "MQTT auth requires TLS" in result.stderr


def test_mqtt_helpers_include_tls_auth_options(tmp_path):
    fakebin = tmp_path / "fakebin"
    fakebin.mkdir()
    args_file = _fake_mqtt_clients(fakebin)
    system_dir, cert_dir, env = _auth_system(tmp_path, fakebin)
    env["FAKE_MQTT_ARGS"] = str(args_file)

    subprocess.run(["mqtt_send", "/allowed-node/probe", "hello"], cwd=system_dir, env=env, check=True)
    _assert_client_uses_tls_auth(args_file, cert_dir)

    subprocess.run(["mqtt_listen", "/allowed-node/probe"], cwd=system_dir, env=env, check=True)
    _assert_client_uses_tls_auth(args_file, cert_dir)

    env["FAKE_MQTT_OUTPUT"] = "on"
    subprocess.run(
        ["mqtt_action", "/allowed-node/probe", "payload", "on", "true"],
        cwd=system_dir,
        env=env,
        check=True,
    )
    _assert_client_uses_tls_auth(args_file, cert_dir)

    env["FAKE_MQTT_OUTPUT"] = "iotempower/_cfg_/test-node/ip 192.0.2.55"
    subprocess.run(["get_ips", "test-node"], cwd=system_dir, env=env, check=True)
    _assert_client_uses_tls_auth(args_file, cert_dir)


def test_prepare_build_dir_escapes_generated_mqtt_and_wifi_config(tmp_path):
    system_dir = tmp_path / "system"
    node_dir = system_dir / "review-node"
    cert_dir = system_dir / "certs"
    local_dir = tmp_path / "local"
    node_dir.mkdir(parents=True)
    cert_dir.mkdir()
    local_dir.mkdir()
    (cert_dir / "ca.crt").write_text(
        "-----BEGIN CERTIFICATE-----\nREVIEWTEST\n-----END CERTIFICATE-----\n",
        encoding="utf-8",
    )
    (system_dir / "system.conf").write_text(
        'IOTEMPOWER_AP_NAME="review-ap"\n'
        'IOTEMPOWER_AP_PASSWORD="wifi\\"pass\\\\test"\n'
        'IOTEMPOWER_AP_IP="192.0.2.1"\n'
        'IOTEMPOWER_MQTT_HOST="broker\\"host"\n'
        "IOTEMPOWER_MQTT_USE_TLS=1\n"
        f'IOTEMPOWER_MQTT_CERT_FOLDER="{cert_dir}"\n'
        'IOTEMPOWER_MQTT_USER="user\\"name"\n'
        'IOTEMPOWER_MQTT_PW="pw\\\\with\\"quotes"\n'
        'IOTEMPOWER_MQTT_DISCOVERY_PREFIX="disc\\"prefix"\n',
        encoding="utf-8",
    )
    (node_dir / "node.conf").write_text('board="wemos d1 mini"\ntopic="review/node"\n', encoding="utf-8")
    (node_dir / "setup.cpp").write_text("// minimal review setup\nvoid setup_iot() {}\n", encoding="utf-8")
    (node_dir / "key.txt").write_text("0" * 64 + "\n", encoding="utf-8")
    env = os.environ.copy()
    env.update(
        {
            "IOTEMPOWER_ACTIVE": "yes",
            "IOTEMPOWER_ROOT": str(REPO_ROOT),
            "IOTEMPOWER_LOCAL": str(local_dir),
            "IOTEMPOWER_COMPILE_CACHE": str(tmp_path / "compile_cache"),
        }
    )

    subprocess.run([str(REPO_ROOT / "bin" / "prepare_build_dir")], cwd=node_dir, env=env, check=True)

    config_h = (node_dir / "build" / "src" / "config.h").read_text(encoding="utf-8")
    wifi_config_h = (node_dir / "build" / "src" / "wifi-config.h").read_text(encoding="utf-8")
    assert '#define mqtt_server "broker\\"host"' in config_h
    assert '#define mqtt_user "user\\"name"' in config_h
    assert '#define mqtt_password "pw\\\\with\\"quotes"' in config_h
    assert '#define mqtt_discovery_prefix "disc\\"prefix"' in config_h
    assert '#define WIFI_PASSWORD "wifi\\"pass\\\\test"' in wifi_config_h


def test_prepare_build_dir_rejects_partial_mqtt_credentials(tmp_path):
    system_dir = tmp_path / "system"
    node_dir = system_dir / "partial-node"
    local_dir = tmp_path / "local"
    node_dir.mkdir(parents=True)
    local_dir.mkdir()
    (system_dir / "system.conf").write_text(
        'IOTEMPOWER_AP_NAME="review-ap"\n'
        'IOTEMPOWER_AP_PASSWORD="review-pass"\n'
        'IOTEMPOWER_AP_IP="192.0.2.1"\n'
        'IOTEMPOWER_MQTT_HOST="broker"\n'
        "IOTEMPOWER_MQTT_USE_TLS=0\n"
        'IOTEMPOWER_MQTT_USER="user-without-password"\n',
        encoding="utf-8",
    )
    (node_dir / "node.conf").write_text('board="wemos d1 mini"\ntopic="partial/node"\n', encoding="utf-8")
    (node_dir / "setup.cpp").write_text("void setup_iot() {}\n", encoding="utf-8")
    (node_dir / "key.txt").write_text("0" * 64 + "\n", encoding="utf-8")
    env = os.environ.copy()
    env.update(
        {
            "IOTEMPOWER_ACTIVE": "yes",
            "IOTEMPOWER_ROOT": str(REPO_ROOT),
            "IOTEMPOWER_LOCAL": str(local_dir),
            "IOTEMPOWER_COMPILE_CACHE": str(tmp_path / "compile_cache"),
        }
    )

    result = subprocess.run(
        [str(REPO_ROOT / "bin" / "prepare_build_dir")],
        cwd=node_dir,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    assert result.returncode != 0
    assert "MQTT user and password must both be set" in result.stderr
