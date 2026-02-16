from textual.widgets import  Button, Input
from textual.screen import Screen
from textual.app import ComposeResult
from pathlib import Path
from messages.deploy_success_message import DeploySuccess
from messages.deploy_failed_message import DeployFailed
from messages.refresh_screen import Refresh
from textual import events, on
from script_activation_logic.new_node_script import deploy_script
from threading import Thread
from screens.loading_screen import LoadingScreen
class NewFolder(Screen):
    def __init__(self, start_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(start_path or Path.cwd())
    def compose(self) -> ComposeResult:
        yield Input(placeholder="Input the new Node name:", id="name", value="new-node")
        yield Button("Submit", id="submit_folder_name")
        yield Button("No, go back", id="pop")
    @on(Button.Pressed, "#submit_folder_name")
    def folder_creating(self)-> None:
        name=self.query_one("#name", Input).value
        self.app.push_screen(LoadingScreen())
        def task():
                answer = deploy_script(self.current_path, name)
                if answer[1] !="":
                     self.post_message(DeployFailed(answer[1], answer[2]))
                else:
                    self.post_message(DeploySuccess(answer[0]))
                    self.post_message(Refresh())

        Thread(target=task, daemon=True).start()