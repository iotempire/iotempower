import os

from tests.install_checks import get_install_check_commands


def test_node_red_npm_checks_both_locations():
    commands = get_install_check_commands("@flowfuse/node-red-dashboard", "npm", "node_red", "/tmp/local")
    assert commands == [
        "cd /tmp/local/nodejs && npm list @flowfuse/node-red-dashboard",
        f"cd {os.path.expanduser('~')}/.node-red && npm list @flowfuse/node-red-dashboard",
    ]


def test_non_node_red_npm_checks_local_nodejs_only():
    commands = get_install_check_commands("terminal-kit", "npm", "general", "/tmp/local")
    assert commands == ["cd /tmp/local/nodejs && npm list terminal-kit"]
