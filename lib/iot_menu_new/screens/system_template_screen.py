from textual.widgets import Label, Button, Input
from textual.screen import Screen
from textual.app import ComposeResult
from pathlib import Path
from messages.deploy_success_message import DeploySuccess
from messages.deploy_failed_message import DeployFailed
from textual import events, on
from screens.loading_screen import LoadingScreen
from script_activation_logic.create_system_logic import create_template
from threading import Thread

class SystemTemplate(Screen):
    def __init__(self, current_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(current_path or Path.cwd())
    def compose(self) -> ComposeResult:
        yield Label("You are about to create a folder called for a new IoT system"  \
         " Do not forget to rename this folder after"   \
         " this creation. If you are using cloudcmd, you might have to refresh to see the newly"    \
         " created folder after this action. It will be created inside the iot-systems folder",)
        yield Button("Create system template", id="template")
        yield Button("Go back", id="pop")
    @on(Button.Pressed,"#template")
    def new_system(self)->None:
            self.app.push_screen(LoadingScreen())

            def task():
                answer = create_template(self.current_path)
                if answer[1] !="":
                     self.post_message(DeployFailed(answer[1], answer[2]))
                else:
                    self.post_message(DeploySuccess(answer[0]))

            Thread(target=task, daemon=True).start()
