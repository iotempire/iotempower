import subprocess
import textwrap
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PARSER = ROOT / "bin" / "config_parser"


def run_bash(script):
    return subprocess.run(
        ["bash"],
        input=script,
        text=True,
        capture_output=True,
        check=False,
    )


def test_system_config_is_inert_and_preserves_shell_syntax(tmp_path):
    marker = tmp_path / "executed"
    config = tmp_path / "system.conf"
    config.write_text(
        textwrap.dedent(
            f"""\
            # comments and blank lines are ignored

            IOTEMPOWER_AP_NAME="name with spaces" # inline comment
            IOTEMPOWER_MQTT_HOST="$(touch {marker})"
            IOTEMPOWER_MQTT_PW='`touch {marker}`'
            export IOTEMPOWER_WEBREPL_PW=$HOME
            """
        )
    )

    result = run_bash(
        textwrap.dedent(
            f"""\
            set -e
            source "{PARSER}"
            iotempower_read_config system "{config}"
            printf '%s\\n' "$IOTEMPOWER_AP_NAME"
            printf '%s\\n' "$IOTEMPOWER_MQTT_HOST"
            printf '%s\\n' "$IOTEMPOWER_MQTT_PW"
            printf '%s\\n' "$IOTEMPOWER_WEBREPL_PW"
            export -p | grep -q 'IOTEMPOWER_WEBREPL_PW'
            """
        )
    )

    assert result.returncode == 0, result.stderr
    assert result.stdout.splitlines() == [
        "name with spaces",
        f"$(touch {marker})",
        f"`touch {marker}`",
        "$HOME",
    ]
    assert not marker.exists()


def test_node_config_accepts_only_expected_node_keys(tmp_path):
    config = tmp_path / "node.conf"
    config.write_text(
        textwrap.dedent(
            """\
            board="wemos d1 mini"
            topic='room/sensor'
            mqtt_server=broker
            mqtt_user=user
            mqtt_password='pass word'
            """
        )
    )

    result = run_bash(
        textwrap.dedent(
            f"""\
            set -e
            source "{PARSER}"
            iotempower_read_config node "{config}"
            printf '%s\\n' "$board" "$topic" "$mqtt_server" "$mqtt_user" "$mqtt_password"
            """
        )
    )

    assert result.returncode == 0, result.stderr
    assert result.stdout.splitlines() == [
        "wemos d1 mini",
        "room/sensor",
        "broker",
        "user",
        "pass word",
    ]


def test_rejects_custom_key_without_partial_assignment(tmp_path):
    config = tmp_path / "system.conf"
    config.write_text(
        textwrap.dedent(
            """\
            IOTEMPOWER_AP_NAME=new-name
            CUSTOM_KEY=value
            """
        )
    )

    result = run_bash(
        textwrap.dedent(
            f"""\
            source "{PARSER}"
            IOTEMPOWER_AP_NAME=original
            iotempower_read_config system "{config}"
            status=$?
            printf '%s\\n' "$status" "$IOTEMPOWER_AP_NAME"
            """
        )
    )

    assert result.returncode == 0
    assert result.stdout.splitlines() == ["1", "original"]
    assert "unsupported key 'CUSTOM_KEY' for system config" in result.stderr


def test_rejects_ambiguous_whitespace_and_unsupported_mode(tmp_path):
    config = tmp_path / "system.conf"
    config.write_text("IOTEMPOWER_AP_NAME=bad value\n")

    whitespace_result = run_bash(
        textwrap.dedent(
            f"""\
            source "{PARSER}"
            iotempower_read_config system "{config}"
            """
        )
    )
    assert whitespace_result.returncode == 1
    assert "unquoted values may not contain whitespace" in whitespace_result.stderr

    empty_config = tmp_path / "empty.conf"
    empty_config.write_text("")
    mode_result = run_bash(
        textwrap.dedent(
            f"""\
            source "{PARSER}"
            iotempower_read_config wifi "{empty_config}"
            """
        )
    )
    assert mode_result.returncode == 1
    assert "unsupported config mode 'wifi'" in mode_result.stderr
