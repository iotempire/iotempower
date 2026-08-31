from textual.app import ComposeResult
from textual.widgets import Log, Button
from textual.containers import Vertical
from textual.screen import Screen

import asyncio
from pathlib import Path


class UpgradeIot(Screen):
    CSS = "Log {height: 1fr;}"

    def compose(self) -> ComposeResult:
        yield Vertical(
            Log(id="log"),
            Button("Upgrade IoT", id="run"),
            Button("Go back", id="pop"),
        )

    async def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "run":
            # Käivita käsu streamimine asünkroonselt
            self.run_worker(self.stream_command(), exclusive=True)


    async def stream_command(self):
        log: Log = self.query_one("#log")

        # Käivitame käsu "iot upgrade"
        process = await asyncio.create_subprocess_exec(
            "iot", "upgrade",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
            cwd=Path.cwd()
        )

        assert process.stdout is not None
        while True:
            line = await process.stdout.readline()
            if not line:
                break

            text = line.decode(errors="replace")
            # Jagame read eraldi ja kuvame Logis
            for subline in text.splitlines() or [""]:
                log.write_line(subline, scroll_end=True)

        await process.wait()
        log.write_line(f"Process finished with code {process.returncode}", scroll_end=True)
