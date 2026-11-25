import subprocess
from pathlib import Path

def deploy_script(path: Path, ssid, password, retry, ip, gateway):
    script = Path(__file__).parent.parent.parent.parent / "bin/wifi_setup_systemconf"

    # All inputs in order
    inputs = [ssid, password, retry, "yes", gateway, ip]

    # Compose input string at once
    input_data = "\n".join(inputs) + "\n"

    process = subprocess.Popen(
        ["bash", str(script)],
        cwd=path,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1
    )

    # Write the entire input
    try:
        process.stdin.write(input_data)
        process.stdin.flush()
    except BrokenPipeError:
        print("Script terminated before all inputs were consumed")

    # Read stdout and stderr in real time
    stdout_lines = []
    stderr_lines = []

    for line in process.stdout:
        print("[STDOUT]", line, end="")
        stdout_lines.append(line)

    for line in process.stderr:
        print("[STDERR]", line, end="")
        stderr_lines.append(line)

    process.wait()
    return "".join(stdout_lines), "".join(stderr_lines), process.returncode