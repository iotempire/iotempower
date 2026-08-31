from textual import on
from textual.screen import Screen
from pathlib import Path
from textual.widgets import Button, Input
from textual.app import ComposeResult
from messages.deploy_success_message import DeploySuccess
from screens.status.loading_screen import LoadingScreen
from script_activation_logic.mqtt.mqtt_listen_logic import open_listener

class MqttListenScreen(Screen):
    def __init__(self,openwrt: bool, current_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(current_path or Path.cwd())
            self.openwrt=openwrt
    def compose(self) -> ComposeResult:
        yield Input(placeholder="Please insert the topic:", id="topic")
        yield Input(placeholder="The MQTT IP:", id="mqtt_ip", value="localhost")
        yield Button("Submit", id="mqtt_listen_message")
        yield Button("Go back", id="pop")
    @on(Button.Pressed, "#mqtt_listen_message")
    def mqtt_listen_screen_logic(self) -> None:
        self.app.push_screen(LoadingScreen())
        topic = self.query_one("#topic", Input).value
        mqtt_ip= self.query_one("#mqtt_ip", Input).value
        open_listener(topic, mqtt_ip)
        self.app.post_message(DeploySuccess("Shell has been opened for topic: " + topic))

