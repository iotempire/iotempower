from textual.widgets import Button, Input
from textual.screen import Screen
from textual.app import ComposeResult
from messages.deploy_success_message import DeploySuccess
from textual import events, on
from script_activation_logic.wifi_setup_systemconf_script import deploy_script
from script_activation_logic.open_wrt_setup_script import router_deploy
from pathlib import Path
from screens.loading_screen import LoadingScreen
from messages.deploy_failed_message import DeployFailed
from messages.deploy_success_message import DeploySuccess
from threading import Thread


class WifiSetupSystemconf(Screen):
    def __init__(self,openwrt: bool, current_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(current_path or Path.cwd())
            self.openwrt=openwrt
    def compose(self) -> ComposeResult:
        yield Input(placeholder="Please choose a SSID for your gateway:", id="SSID")
        yield Input(placeholder="Please create a new password:", id="pass")
        yield Input(placeholder="Please confirm your new password:", id="new_pass")
        yield Input(placeholder="The MQTT host IP:", id="ip")
        yield Input(placeholder="The default Accesspoint host IP for external router is", id="gateway", value="192.168.14.1")
        yield Button("Submit", id="wifi_systemconf_submit")
        yield Button("Go back", id="pop")
    @on(Button.Pressed, "#wifi_systemconf_submit")
    def wifi_systemconf_logic(self)-> None:
        ssid=self.query_one("#SSID", Input).value
        password=self.query_one("#pass", Input).value
        retry=self.query_one("#new_pass", Input).value
        ip=self.query_one("#ip", Input).value
        gateway=self.query_one("#gateway", Input).value
        self.app.push_screen(LoadingScreen())
        def task():
            if not self.openwrt:
                answer =deploy_script(self.current_path,ssid,password,retry,ip,gateway)
            else:
                 answer =router_deploy(self.current_path,ssid,password,retry,ip,gateway)
            if answer[1] !="":
                    self.post_message(DeployFailed(answer[1], answer[2]))
            else:
                self.post_message(DeploySuccess(answer[0]))

        Thread(target=task, daemon=True).start()