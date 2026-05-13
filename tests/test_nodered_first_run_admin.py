import os
import subprocess
from pathlib import Path

import pytest


REPO_ROOT = Path(__file__).resolve().parents[1]
ENSURE_ADMIN_AUTH = REPO_ROOT / "bin" / "nodered_ensure_admin_auth"
PASSWORD_CLI = REPO_ROOT / "bin" / "nodered_password_cli.js"

CUSTOM_ADMIN_AUTH_FORMS = [
    pytest.param(
        'module.exports = {\n    adminAuth: { type: "credentials" }\n};\n',
        id="bare-key",
    ),
    pytest.param(
        'module.exports = {\n    "adminAuth": { type: "credentials" }\n};\n',
        id="quoted-key",
    ),
    pytest.param(
        'module.exports["adminAuth"] = { type: "credentials" };\n',
        id="bracket-assignment",
    ),
    pytest.param(
        'module.exports.adminAuth = { type: "credentials" };\n',
        id="object-property",
    ),
    pytest.param(
        'module.exports = { flowFile: "flows.json", adminAuth: { type: "credentials" } };\n',
        id="same-line-bare-key",
    ),
    pytest.param(
        'module.exports = {}; module.exports["adminAuth"] = { type: "credentials" };\n',
        id="same-line-bracket-assignment",
    ),
]


def fake_iotempower_local(tmp_path):
    local_dir = tmp_path / "local"
    bcrypt_dir = local_dir / "nodejs" / "node_modules" / "bcryptjs"
    bcrypt_dir.mkdir(parents=True, exist_ok=True)
    (bcrypt_dir / "index.js").write_text(
        "\n".join(
            [
                "module.exports = {",
                '    hashSync: function(password) { return "hash-for-" + password; },',
                "    compareSync: function() { return true; }",
                "};",
                "",
            ]
        ),
        encoding="utf-8",
    )
    return local_dir


def iotempower_env(tmp_path):
    env = os.environ.copy()
    env.update(
        {
            "IOTEMPOWER_ACTIVE": "yes",
            "IOTEMPOWER_ROOT": str(REPO_ROOT),
            "IOTEMPOWER_LOCAL": str(fake_iotempower_local(tmp_path)),
            "IOTEMPOWER_NODE_RED_AUTH_QUIET": "1",
        }
    )
    for key in (
        "IOTEMPOWER_NODE_RED_CREDENTIALS_FILE",
        "IOTEMPOWER_NODE_RED_PASSWORD_HASH_FILE",
        "IOTEMPOWER_NODE_RED_SETTINGS_FILE",
        "IOTEMPOWER_NODE_RED_USER_DIR",
        "IOTEMPOWER_NODE_RED_BCRYPTJS_PATH",
    ):
        env.pop(key, None)
    return env


def run_ensure_admin_auth(settings_file, env):
    return subprocess.run(
        [str(ENSURE_ADMIN_AUTH), str(settings_file)],
        cwd=REPO_ROOT,
        env=env,
        text=True,
        capture_output=True,
    )


@pytest.mark.parametrize("settings_text", CUSTOM_ADMIN_AUTH_FORMS)
def test_ensure_admin_auth_preserves_custom_admin_auth_forms(tmp_path, settings_text):
    settings_file = tmp_path / "settings.js"
    settings_file.write_text(settings_text, encoding="utf-8")

    result = run_ensure_admin_auth(settings_file, iotempower_env(tmp_path))

    assert result.returncode == 0, result.stderr
    assert settings_file.read_text(encoding="utf-8") == settings_text
    assert not (tmp_path / "iotempower-admin-credentials").exists()
    assert not (tmp_path / "iotempower-admin-password.hash").exists()


@pytest.mark.parametrize("settings_text", CUSTOM_ADMIN_AUTH_FORMS)
def test_password_cli_reports_custom_admin_auth_forms(tmp_path, settings_text):
    settings_file = tmp_path / "settings.js"
    settings_file.write_text(settings_text, encoding="utf-8")

    result = subprocess.run(
        ["node", str(PASSWORD_CLI), "reset", str(settings_file)],
        cwd=REPO_ROOT,
        env=iotempower_env(tmp_path),
        text=True,
        capture_output=True,
    )

    assert result.returncode == 1
    assert "custom adminAuth" in result.stderr


@pytest.mark.parametrize(
    "existing_root, existing_name, expected_added",
    [
        pytest.param(
            "module.exports.httpAdminRoot = '/custom-admin';",
            "httpAdminRoot",
            "module.exports.httpNodeRoot = '/nodered';",
            id="preserve-existing-admin-root",
        ),
        pytest.param(
            "module.exports['httpNodeRoot'] = '/custom-node';",
            "httpNodeRoot",
            "module.exports.httpAdminRoot = '/nodered';",
            id="preserve-existing-node-root",
        ),
    ],
)
def test_managed_auth_adds_only_missing_default_http_root(
    tmp_path,
    existing_root,
    existing_name,
    expected_added,
):
    settings_file = tmp_path / "settings.js"
    settings_file.write_text(
        "\n".join(
            [
                existing_root,
                "",
                "/* IOTEMPOWER_NODE_RED_ADMIN_AUTH */",
                "module.exports.adminAuth = {};",
                "",
            ]
        ),
        encoding="utf-8",
    )

    result = run_ensure_admin_auth(settings_file, iotempower_env(tmp_path))
    settings_text = settings_file.read_text(encoding="utf-8")

    assert result.returncode == 0, result.stderr
    assert existing_root in settings_text
    assert expected_added in settings_text
    assert settings_text.count(existing_name) == 1
