import threading
from textual.widgets import Label, Button, Header
from textual.screen import Screen
from textual.app import ComposeResult
from pathlib import Path
from messages.deploy_success_message import DeploySuccess
from textual import events, on
from script_activation_logic.deploy_script import deploy_script
from messages.deploy_failed_message import DeployFailed
from screens.loading_screen import LoadingScreen
class DeployScreen(Screen):
        def __init__(self, current_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(current_path or Path.cwd())
        def compose(self) -> ComposeResult:
            yield Header()
            yield Label("You are baout to deploy from the following path:")
            yield Label(f"{self.current_path}")
            yield Label("Are you sure?")
            yield Button("Yes, run deploy", id="deploy_logic")
            yield Button("No, go back", id="pop")
        @on(Button.Pressed, "#deploy_logic")
        def deployment_logic(self)->None:
            self.app.push_screen(LoadingScreen())

            def task():
                answer = deploy_script(self.current_path)
                if answer[1] !="":
                     self.post_message(DeployFailed(answer[1], answer[2]))
                else:
                    self.post_message(DeploySuccess(answer[0]))

            threading.Thread(target=task, daemon=True).start()

