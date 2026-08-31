import subprocess
from pathlib import Path

def create_template(path: Path):
    script = Path(__file__).parent.parent.parent.parent / "bin/create_system_template"  # locate the script in the project `bin/` by going up four levels from this file
    result = subprocess.run(
        ["bash", str(script)],
        capture_output=True,
        text=True,
        cwd=path
    )
    return [result.stdout, result.stderr, result.returncode]