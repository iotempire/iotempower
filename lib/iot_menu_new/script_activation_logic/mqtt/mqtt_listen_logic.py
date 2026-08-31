import os
import platform
import shutil
import subprocess


def open_listener(topic, ip) -> None:
    """Open a terminal and run the MQTT listener command in it."""
    command = f'mosquitto_sub -h {ip} -t "{topic}"'

    try:
        system = platform.system()

        if system == "Windows":
            try:
                subprocess.Popen(
                    ["wt.exe", "new-tab", "--", "powershell.exe", "-NoExit", "-Command", command],
                    start_new_session=True,
                )
                return
            except FileNotFoundError:
                pass

            subprocess.Popen(
                ["cmd.exe", "/K", command],
                start_new_session=True,
            )
            return

        if system == "Linux":
            if os.environ.get("WSL_INTEROP"):
                try:
                    subprocess.Popen(
                        ["cmd.exe", "/K", command],
                        start_new_session=True,
                    )
                    return
                except Exception:
                    pass

            if shutil.which("ptyxis"):
                subprocess.Popen(
                    [
                        "ptyxis",
                        "--new-window",
                        "--working-directory",
                        str(os.getcwd()),
                        "--",
                        "bash",
                        "-lc",
                        command,
                    ],
                    start_new_session=True,
                )
                return

            if shutil.which("gnome-terminal"):
                subprocess.Popen(
                    ["gnome-terminal", "--", "bash", "-lc", command],
                    start_new_session=True,
                )
                return

            if shutil.which("konsole"):
                subprocess.Popen(
                    ["konsole", "-e", "bash", "-lc", command],
                    start_new_session=True,
                )
                return

            if shutil.which("xfce4-terminal"):
                subprocess.Popen(
                    ["xfce4-terminal", "--hold", "-e", f"bash -lc '{command}'"],
                    start_new_session=True,
                )
                return

            if shutil.which("mate-terminal"):
                subprocess.Popen(
                    ["mate-terminal", "--", "bash", "-lc", command],
                    start_new_session=True,
                )
                return

            if shutil.which("tilix"):
                subprocess.Popen(
                    ["tilix", "-e", "bash", "-lc", command],
                    start_new_session=True,
                )
                return

            if shutil.which("xterm"):
                subprocess.Popen(
                    ["xterm", "-e", "bash", "-lc", command],
                    start_new_session=True,
                )
                return

            subprocess.Popen(["bash", "-lc", command], start_new_session=True)
            return

        if system == "Darwin":
            subprocess.Popen(
                ["open", "-a", "Terminal", "--args", "bash", "-lc", command],
                start_new_session=True,
            )
            return

        raise OSError(f"Unsupported platform: {system}")

    except Exception as e:
        print(f"Failed to open terminal: {e}")
