#!/usr/bin/env python3

import asyncio
from textual.widgets import Static
import subprocess
import re


def update_static(screen, idd, text, append=False) -> None:
    """
    Utility function to update static screen object
	
	Used to update text on screen (once) after receiving it from an async function.
    """

    s = screen.query_one(f'#{idd}', Static)
    newtext = text if not append else s.renderable + text
    s.update(newtext)


def validate_config_params(log, backend, nname, npass, npass2) -> bool:
    """Validate the given params: SSID and password for an AP"""

    if not backend or not nname or not npass or len(nname) < 2 or len(nname) > 32 or len(npass) < 8 or len(npass) > 128:
        log.clear()
        log.write_line('[!] Make sure your network name is valid and password has at least 8 characters!')
        return False

    if npass != npass2:
        log.clear()
        log.write_line('[!] The passwords do not match!')
        return False

    return True


def parse_wifi_creds_output(output):
    """Parse read_wifi_creds.sh key/value output."""

    creds = {}
    for line in output.splitlines():
        key, sep, value = line.partition("=")
        if sep and key in {"SSID", "Password", "GatewayIP"}:
            creds[key] = value
    return creds


async def run_cmd_async(cmd, bg=False):
    """Run the given command asynchronously and return output"""

    if isinstance(cmd, (list, tuple)):
        proc = await asyncio.create_subprocess_exec(
            *cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    else:
        proc = await asyncio.create_subprocess_shell(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

    # If bg=True, launch silently in background
    if not bg:
        stdout, stderr = await proc.communicate()
        return stdout.decode(), stderr.decode()
    else:
        asyncio.create_task(proc.communicate())
        return None


def extract_keywords(strng):
	"""Given a string, extract keywords in the form of key:value;"""

	pattern = r'(\w+):([^;]*)'
	matches = re.findall(pattern, strng)
	return matches
