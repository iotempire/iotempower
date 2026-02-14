from textual.app import ComposeResult
from textual.widgets import Log, Button
from textual.containers import Vertical
from textual.screen import Screen

import asyncio
from pathlib import Path


class InitializeSerial(Screen):
    CSS = "Log {height: 1fr;}"

    def __init__(self, current_path: str = None, **kwargs):
        super().__init__(**kwargs)
        self.current_path = Path(current_path or Path.cwd())

    def compose(self) -> ComposeResult:
        yield Vertical(
            Log(id="log"),
            Button("Initialize serial", id="run"),
            Button("Go back", id="pop"),
        )

    async def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "run":
            # Käivita stream_script asünkroonselt
            self.run_worker(self.stream_script(), exclusive=True)

    async def stream_script(self):
        log: Log = self.query_one("#log")

        script = Path(__file__).parent.parent.parent.parent / "bin/initialize"
        process = await asyncio.create_subprocess_exec(
            "bash", str(script),"serial",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
            cwd=self.current_path
        )

        assert process.stdout is not None
        while True:
            line = await process.stdout.readline()
            if not line:
                break

            text = line.decode(errors="replace")
            # Jagame read eraldi ja kirjutame write_line-iga
            for subline in text.splitlines() or [""]:
                log.write_line(subline, scroll_end=True)

        await process.wait()
        log.write_line(f"Process finished with code {process.returncode}", scroll_end=True)