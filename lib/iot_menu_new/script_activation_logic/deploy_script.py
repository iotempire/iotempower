import subprocess
from pathlib import Path

def deploy_script(path: Path):
    script = Path(__file__).parent.parent.parent.parent / "bin/deploy"  # takes 'deploy' script two levels up from this file
    result = subprocess.run(
        ["bash", str(script)],
        capture_output=True,
        text=True,
        cwd=path
    )
    return [result.stdout, result.stderr, result.returncode]

