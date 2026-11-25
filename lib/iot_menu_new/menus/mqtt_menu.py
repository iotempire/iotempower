from textual.widgets import  Button
from textual.containers import VerticalScroll
from textual.app import ComposeResult

class MqttMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Listen", id="mqtt_listen")
        yield Button("Publish", id="mqtt_publish")
        yield Button("Back", id="back")