from textual.widgets import Label, Button
from textual.screen import Screen
from textual.app import ComposeResult
class Failed(Screen):
    def __init__(self, error: str, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.error = error  # store the error message in the instance
        self.code=1
    def compose(self) -> ComposeResult:
        yield Label("Failed")
        yield Label(f"With message: {self.error}Code: {self.code}")
        yield Button("Go back", id="pop")