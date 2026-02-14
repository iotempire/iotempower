from textual.widgets import  Button
from textual.containers import VerticalScroll
from textual.app import ComposeResult

class BasicMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Deploy", id="deploy")
        yield Button("Adopt", id="adopt")
        yield Button("Wifi Network Setup", id="wifi")
        yield Button("Create New Folder",id="folder")
        yield Button("Advanced", id="advanced")
        yield Button("Web starter",id="web_starter")
        yield Button("AP Configurator", id="ap_configurator")
        yield Button("Exit", id="exit")