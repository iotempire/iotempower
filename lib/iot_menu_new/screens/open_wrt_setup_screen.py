from textual.widgets import Label, Button, Input
from textual.screen import Screen
from textual.app import ComposeResult
from pathlib import Path
from messages.deploy_success_message import DeploySuccess
from textual import events, on


class OpenwrtSetup(Screen):
    def compose(self) -> ComposeResult:
        yield Label("!!!Make sure you are connected to your router via Ethernet or Wifi!!! " \
        "Make sure your router has internet connection^" \
        "This will create a file that pre configure your gateway credentials. Please" \
        "define the SSID and password for your external WiFi-router running OpenWRT. This" \
        "Wifi credentials will become your default settings." \
        "Do you want to continue editing your Gateway's WiFi configuration?")
        yield Button("Submit", id="openwrt_submit")
        yield Button("Go back", id="pop")