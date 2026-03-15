from textual.widgets import Button
from textual.containers import VerticalScroll
from textual.app import ComposeResult

class WifiMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Wifi system credentials setup - system.conf file", id="wifi_conf")
        yield Button("Wifi network initial setup - OpenWRT router", id="openwrt")
        yield Button("Update Wifi Credentials on OpenWRT router")
        yield Button("Set-up wifi gateway on the Raspberry Pi")
        yield Button("Back", id="back")