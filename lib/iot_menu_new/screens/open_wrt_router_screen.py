from textual.widgets import Label, Button, Input
from textual.screen import Screen
from textual.app import ComposeResult
from pathlib import Path
from messages.deploy_success_message import DeploySuccess
from textual import events, on
from script_activation_logic.new_ip_logic import router_ip
from screens.loading_screen import LoadingScreen
from messages.deploy_failed_message import DeployFailed
from threading import Thread
from screens.wifi_setup_system_conf_screen import WifiSetupSystemconf

class OpenWrtRouterIp(Screen):
    def __init__(self, info, path: Path):
        super().__init__()
        self.info=info
        self.path=path
    def compose(self) -> ComposeResult:
        yield Label(f"{self.info[0]}")
        yield Button("Correct ip", id="openwrt_wifi")
        yield Input("New ip", id="new_ip_I")
        yield Button("Change ip", id="new_ip")
        yield Button("Go back to Menu", id="pop")
    @on(Button.Pressed, "#new_ip")
    def add_new_ip(self) -> None:
        self.app.push_screen(LoadingScreen)
        ip=self.query_one("#new_ip_I")
        def task():
            answer = router_ip(ip)
            if answer[1] !="":
                    self.post_message(DeployFailed(answer[1], answer[2]))
            else:
                self.app.push_screen(OpenWrtRouterIp(answer))

        Thread(target=task, daemon=True).start()
    @on(Button.Pressed, "#openwrt_wifi")
    def add_new_ip(self) -> None:
        self.app.pop_screen()
        self.app.push_screen(WifiSetupSystemconf(True,self.path))