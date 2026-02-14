from textual.screen import Screen
from textual.widgets import Static

class LoadingScreen(Screen):
    def compose(self):
        yield Static("⏳ Deploy is running... Please wait.", id="loading_message")
