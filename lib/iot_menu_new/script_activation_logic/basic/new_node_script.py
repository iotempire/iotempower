import subprocess
from pathlib import Path

def deploy_script(path: Path, name):
    script = Path(__file__).parent.parent.parent.parent / "bin/create_node_template"  # locate the script in the project `bin/` by going up four levels from this file
    result = subprocess.run(
        ["bash", str(script),name],
        capture_output=True,
        text=True,
        cwd=path
    )
    return [result.stdout, result.stderr, result.returncode]