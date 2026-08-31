import subprocess
from pathlib import Path


def run_adopt_scan(path: Path) -> list[str]:
    """Run the deploy/adopt scripts and return the parsed list of nodes."""
    dongle = Path(__file__).parent.parent.parent.parent / "bin/dongle"
    scan = Path(__file__).parent.parent.parent.parent / "bin/scan"
    result = subprocess.run(
            [str(dongle), str(scan)],
            capture_output=True,
            text=True,
            cwd=path
        )
    saved_output = result.stdout.splitlines()

    ap_list = []
    for line in saved_output:
        if line.strip():
            space_split = line.split(" ")
            parts = space_split[0].split("-")
            ap_list.append(
                f"node {parts[2]}: {parts[3][0]} long, {parts[4][0]} short blinks"
            )

    return ap_list
def adopt_node(selection: str):
    """Run the actual 'adopt' command for the given node."""
    print(f"You are about to adopt the node {selection}")
    # here you can place the same shell_command_in_path logic
    adopt = Path(__file__).parent.parent.parent/"adopt"
    subprocess.run([str(adopt), selection])
