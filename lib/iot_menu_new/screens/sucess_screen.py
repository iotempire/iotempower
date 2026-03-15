from textual.widgets import Label, Button
from textual.screen import Screen
from textual.app import ComposeResult
class Success(Screen):
    def __init__(self, message: str, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.message = message  # store the message in the instance
    def compose(self) -> ComposeResult:
        yield Label("Success")
        yield Label("With message: "+self.message)
        yield Button("Go back", id="pop")