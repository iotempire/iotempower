from pathlib import Path
from textual import  on
from textual.app import ComposeResult
from textual.screen import Screen
from textual.widgets import Button
from script_activation_logic.advanced.open_shell_logic import  open_shell_logic

class ShellScreen(Screen):
    def __init__(self, current_path: str | None = None, **kwargs):
        super().__init__(**kwargs)
        self.current_path = Path(current_path or Path.cwd())

    def compose(self) -> ComposeResult:
        yield Button("Open Shell", id="open_shell")
        yield Button("Go back", id="pop")

    @on(Button.Pressed, "#open_shell")
    def open_shell2(self) -> None:
        open_shell_logic(self)