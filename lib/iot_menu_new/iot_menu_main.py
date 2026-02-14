from pathlib import Path
from textual.app import App, ComposeResult
from textual.widgets import DirectoryTree, Header, Footer, Static, Button, Label, Input
from textual.binding import Binding
from textual import events, on
from textual.containers import Container, VerticalScroll, Horizontal
from textual.message import Message

from textual.screen import Screen


class Success(Screen):
    def compose(self) -> ComposeResult:
        yield Label("Success")
        yield Button("Go back", id="pop")

class DeploySuccess(Message):
    pass


class BasicMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Deploy", id="deploy")
        yield Button("Adopt", id="adopt")
        yield Button("Wifi Network Setup", id="wifi")
        yield Button("Create New Folder",id="folder")
        yield Button("Advanced", id="advanced")
        yield Button("Exit", id="exit")

class AdvancedMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Pre-Flash Wemos D1 Mini")
        yield Button("Initialize Serial")
        yield Button("Upgrade")
        yield Button("Create New System")
        yield Button("Shell Escape")
        yield Button("Shutdown/Poweroff")
        yield Button("Back", id="back")

class WifiMenu(VerticalScroll):
    def compose(self) -> ComposeResult:
        yield Button("Wifi system credentials setup - system.conf file", id="wifi_conf")
        yield Button("Wifi network initial setup - OpenWRT rouyter", id="openwrt")
        yield Button("Update Wifi Credentials on OpenWRT router")
        yield Button("Set-up wifi gateway on the Raspberry Pi")
        yield Button("Back", id="back")

class DeployScreen(Screen):
        def __init__(self, start_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(start_path or Path.cwd())
        def compose(self) -> ComposeResult:
            yield Header()
            yield Label("You are baout to deploy from the following path:")
            yield Label(f"{self.current_path}")
            yield Label("Are you sure?")
            yield Button("Yes, run deploy", id="deploy_logic")
            yield Button("No, go back", id="pop")
        @on(Button.Pressed, "#deploy_logic")
        def deploymentLogic(self)->None:
            self.post_message(DeploySuccess())
            self.app.pop_screen()

class AdoptScreen(Screen):
    def __init__(self, start_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(start_path or Path.cwd())
    def compose(self) -> ComposeResult:
        ##to do
        yield Button("No, go back", id="pop")


class NewFolder(Screen):
    def __init__(self, start_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(start_path or Path.cwd())
    def compose(self) -> ComposeResult:
        yield Input(placeholder="Input the new Node name:")
        yield Button("Submit", id="submit_folder_name")
        yield Button("No, go back", id="pop")
    @on(Button.Pressed, "#submit_folder_name")
    def folderCreating(self)-> None:
        ##todo: create folder logic
        self.post_message(DeploySuccess())
        self.app.pop_screen()
        
class WifiSetupSystemconf(Screen):
    def compose(self) -> ComposeResult:
        yield Input(placeholder="Please choose a SSID for your gateway:", id="SSID")
        yield Input(placeholder="Please create a new password:", id="pass")
        yield Input(placeholder="Please confirm your new password:", id="new_pass")
        yield Input(placeholder="The MQTT host IP:", id="ip")
        yield Button("Submit", id="wifi_systemconf_submit")
        yield Button("Go back", id="pop")
    @on(Button.Pressed, "#wifi_systemconf_submit")
    def WifiSystemconfLogic(self)-> None:
        ssid=self.query_one("#SSID", Input).value
        password=self.query_one("#pass", Input).value
        retry=self.query_one("#new_pass", Input).value
        ip=self.query_one("#ip", Input).value
        ##todo logic
        self.post_message(DeploySuccess())
        self.app.pop_screen()

class OpenwrtSetup(Screen):
    def compose(self) -> ComposeResult:
        yield Label("!!!Make sure you are connected to your router via Ethernet or Wifi!!! " \
        "Make sure your router has internet connection^" \
        "This will create a file that pre configure your gateway credentials. Please" \
        "define the SSID and password for your external WiFi-router running OpenWRT. This" \
        "Wifi credentials will become your default settings." \
        "Do you want to continue editing your Gateway's WiFi configuration?")
        yield Input(placeholder="Please choose a SSID for your gateway:", id="ip_openwrt")
        yield Button("Submit", id="openwrt_submit")
        yield Button("Go back", id="pop")
    @on(Button.Pressed, "#openwrt_submit")
    def OpenWrtLogic(self)-> None:
        ssid=self.query_one("#ip_openwrt", Input).value

                    ##todo logic
        self.post_message(DeploySuccess())
        self.app.pop_screen()



class IotMenu(App[None]):
    SCREENS= {
        "deploy": DeployScreen,
        "adopt": AdoptScreen,
        "folder": NewFolder,
        "wifi_conf": WifiSetupSystemconf,
        "openwrt": OpenwrtSetup

    }
    CSS_PATH = "tcss/iot_menu.tcss"
    """
    A Textual-based file manager without directory history.
    - Arrow keys to move
    - Enter to expand directories; if already expanded, pressing Enter again navigates into it
    - Backspace to go up
    """
    BINDINGS = [
        Binding("enter", "noop", "Expand or navigate directory", show=False),  # handled in on_key
        Binding("backspace", "up", "Go up", show=True),
    ]

    def __init__(self, start_path: str = None, **kwargs):
        super().__init__(**kwargs)
        self.current_path = Path(start_path or Path.cwd())

    def compose(self) -> ComposeResult:
        yield Header()
        with Horizontal():
            with Container(id="left_panel"):
                yield Static(f"Current Path: {self.current_path}", id="path_display")
                yield DirectoryTree(self.current_path, id="dir_tree")
            yield Container(BasicMenu(), id="right_panel")       
        yield Footer()

    @on(DeploySuccess)
    def sucess_page(self)-> None:
        self.push_screen(Success())



    @on(Button.Pressed, "#deploy,#adopt,#folder,#wifi_conf, #openwrt")
    def action_deployment_screen(self, event: Button.Pressed)-> None:
        self.push_screen(event.button.id)



    @on(Button.Pressed, "#advanced")
    def action_remove_Basic_menu_and_add_Advanced(self)-> None:
        new_Advanced_menu=AdvancedMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_Advanced_menu)


    @on(Button.Pressed, "#back")
    def action_remove_menu_and_add_Basic(self)-> None:
        new_Basic_menu=BasicMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_Basic_menu)

    @on(Button.Pressed, "#wifi")
    def action_remove_Basic_menu_and_add_wifi(self)-> None:
        new_Wifi_menu=WifiMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_Wifi_menu)


    @on(Button.Pressed, "#pop")
    def pop_screen_new(self)->None:
        self.pop_screen()


    @on(Button.Pressed, "#exit")
    def exit_the_app(self)->None:
        self.exit()

    def on_mount(self) -> None:
        self.dir_tree = self.query_one("#dir_tree", DirectoryTree)
        self.path_display = self.query_one("#path_display", Static)
        self.set_focus(self.dir_tree)

    def on_key(self, event: events.Key) -> None:
        if event.key == "enter" and self.focused is self.dir_tree:
            node = self.dir_tree.cursor_node
            entry = node.data
            path = Path(entry.path)
            if path.is_dir():
                if node.is_expanded:
                    # Already expanded: navigate into it
                    self._load_path(path)
                event.stop()
            else:
                self.log(f"Selected file: {path}")
                event.stop()

    def action_up(self) -> None:
        parent = self.current_path.parent
        if parent != self.current_path:
            self._load_path(parent)

    def _load_path(self, path: Path) -> None:
        """Load the given path."""
        self.current_path = path
        self.path_display.update(f"Current Path: {self.current_path}")
        self.dir_tree.path = self.current_path
        self.dir_tree.reload()

if __name__ == "__main__":
    import sys
    start = sys.argv[1] if len(sys.argv) > 1 else None
    IotMenu(start_path=start).run()
