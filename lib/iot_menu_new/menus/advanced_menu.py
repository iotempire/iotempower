from textual.widgets import  Button
from textual.containers import VerticalScroll
from textual.app import ComposeResult

class AdvancedMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Pre-Flash Wemos D1 Mini", id="wemos")
        yield Button("Initialize Serial", id="initialize")
        yield Button("Upgrade", id="upgrade")
        yield Button("Create New System", id="new_system_template")
        yield Button("Shell Escape")
        yield Button("Shutdown/Poweroff")
        yield Button("Back", id="back")