# app.py
from __future__ import annotations

import asyncio
from collections import deque
from pathlib import Path
import sys
from pathlib import Path as _Path

# Ensure `${workspace}/lib` is on `sys.path` so imports like
# `import ap_configurator` resolve regardless of current working directory
# (works when running the app from the repo root, from other folders, or
# when launched by the debugger). Insert before importing ap_configurator.
_REPO_LIB = _Path(__file__).resolve().parents[1]
if str(_REPO_LIB) not in sys.path:
    sys.path.insert(0, str(_REPO_LIB))

import ap_configurator
from textual.app import App, ComposeResult
from textual.binding import Binding
from textual import events, on
from textual.containers import Container, Horizontal
from textual.widgets import DirectoryTree, Header, Footer, Static, Button

from screens.sucess_screen import Success
from screens.deploy_screen import DeployScreen
from screens.new_folder_screen import NewFolder
from screens.adopt_screen import AdoptScreen
from screens.wifi_setup_system_conf_screen import WifiSetupSystemconf
from screens.open_wrt_setup_screen import OpenwrtSetup
from screens.failed_screen import Failed
from screens.open_wrt_router_screen import OpenWrtRouterIp
from screens.pre_flash_wemos_d1_mini import WemosPre
from screens.initialize_serial import InitializeSerial
from screens.system_template_screen import SystemTemplate
from screens.upgrade_screen import UpgradeIot
from screens.web_starter_screen import WebStarter
from screens.loading_screen import LoadingScreen  # kui kasutusel

from menus.basic_menu import BasicMenu
from menus.advanced_menu import AdvancedMenu
from menus.wifi_menu import WifiMenu

from messages.refresh_screen import Refresh
from messages.deploy_success_message import DeploySuccess
from messages.deploy_failed_message import DeployFailed
from messages.web_output import WebOutput  # <-- message carrier for log lines

from script_activation_logic.find_router_ip_logic import router_ip



# --- Location of the persistent log file for the web starter
LOG_PATH = Path("web_starter.log")


class IotMenu(App[None]):
    """Textual-based file manager.

    - Arrow keys to move
    - Enter to expand or enter a directory
    - Backspace to go up
    """
    CSS_PATH = "tcss/iot_menu.tcss"

    BINDINGS = [
        Binding("enter", "noop", "Expand or navigate directory", show=False),  # handled in on_key
        Binding("backspace", "up", "Go up", show=True),
    ]

    def __init__(self, start_path: str | None = None, **kwargs):
        super().__init__(**kwargs)
        self.current_path = Path(start_path or Path.cwd())

        # Web starter background process state at the App level
        self.web_starter: asyncio.subprocess.Process | None = None
        self.web_stream_task: asyncio.Task | None = None
        self.web_log_buffer: deque[str] = deque(maxlen=5000)  # keep last N lines

        # SCREENS - create factories so `current_path` can be passed when needed
        self.SCREENS = {
            "deploy": lambda: DeployScreen(self.current_path),
            "adopt": lambda: AdoptScreen(self.current_path),
            "folder": lambda: NewFolder(self.current_path),
            "wifi_conf": lambda: WifiSetupSystemconf(False, self.current_path),
            "openwrt": lambda: OpenwrtSetup(self.current_path),
            "wemos": lambda: WemosPre(),
            "initialize": lambda: InitializeSerial(self.current_path),
            "new_system_template": lambda: SystemTemplate(self.current_path),
            "upgrade": lambda: UpgradeIot(),
            "web_starter": lambda: WebStarter(),
            "ap_configurator": lambda: __import__("iot_menu_2.screens.ap_configurator_screen", fromlist=["APConfiguratorScreen"]).APConfiguratorScreen(),
        }

    # ---------------------------
    # Load log history at startup
    # ---------------------------
    def load_log_history(self, max_lines: int = 1000) -> None:
        """Load previous log lines from the file into the buffer (last N lines)."""
        try:
            if LOG_PATH.exists():
                lines = LOG_PATH.read_text(encoding="utf-8", errors="replace").splitlines()
                # võta ainult viimased N rida
                for line in lines[-max_lines:]:
                    self.web_log_buffer.append(line)
        except Exception as e:
            # Must not abort app startup; optionally log to console
            self.log(f"Failed to load log history: {e}")

    # ---------------------------
    # Web starter: start/stop + stream
    # ---------------------------
    async def start_web_starter(self) -> None:
        """Start the web starter (if not already running) and stream stdout at the App level."""
        if self.web_starter is not None and self.web_starter.returncode is None:
            return  # already running

        # Launch the process
        self.web_starter = await asyncio.create_subprocess_exec(
            "web_starter",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
            # cwd=str(self.current_path),  # vajadusel aktiveeri
        )

        # Log "launched" into buffer and file + send event to screens
        start_msg = f"Web starter launched (PID={self.web_starter.pid})"
        self._append_log_line(start_msg, emit=True)

        async def _stream():
            try:
                assert self.web_starter and self.web_starter.stdout
                async for raw in self.web_starter.stdout:
                    line = raw.decode(errors="replace").rstrip()
                    self._append_log_line(line, emit=True)
            except asyncio.CancelledError:
                # Expected when the stream is stopped
                pass

        self.web_stream_task = asyncio.create_task(_stream())

    async def stop_backend(self) -> None:
        """Stop the stream and process. Use only from Stop button or on app exit."""
        # 1) stop reading the stream
        if self.web_stream_task and not self.web_stream_task.done():
            self.web_stream_task.cancel()
            try:
                await self.web_stream_task
            except asyncio.CancelledError:
                pass
        self.web_stream_task = None

        # 2) end the process
        proc = self.web_starter
        if proc and proc.returncode is None:
            proc.terminate()
            try:
                await asyncio.wait_for(proc.wait(), timeout=3)
            except asyncio.TimeoutError:
                proc.kill()
                await proc.wait()

            # record stop both in file and buffer
            self._append_log_line("Web starter stopped", emit=True)

        self.web_starter = None

    def _append_log_line(self, line: str, *, emit: bool = False) -> None:
        # 1) in-memory
        self.web_log_buffer.append(line)

        # 2) to file (append)
        try:
            LOG_PATH.parent.mkdir(parents=True, exist_ok=True)
            with LOG_PATH.open("a", encoding="utf-8", errors="replace") as f:
                f.write(line + "\n")
        except Exception as e:
            self.log(f"Failed writing log: {e}")

        # 3) send message to active screen (if it listens)
        if emit:
            # Note: when sending App->Screen target the Screen, not the App
            self.call_later(lambda: self.screen.post_message(WebOutput(line)))
            # If you want to broadcast to all screens, use:
            # self.call_later(lambda: [scr.post_message(WebOutput(line)) for scr in list(self.screen_stack)])

    # ---------------------------
    # UI Compose
    # ---------------------------
    def compose(self) -> ComposeResult:
        yield Header()
        with Horizontal():
            with Container(id="left_panel"):
                yield Static(f"Current Path: {self.current_path}", id="path_display")
                yield DirectoryTree(self.current_path, id="dir_tree")
            yield Container(BasicMenu(), id="right_panel")
        yield Footer()

    # ---------------------------
    # Messages / screen transitions
    # ---------------------------
    @on(DeploySuccess)
    def sucess_page(self, message: DeploySuccess) -> None:
        self.pop_screen()
        self.pop_screen()
        self.push_screen(Success(message.message))

    @on(Refresh)
    def refresh_screen(self) -> None:
        self._load_path(self.current_path)

    @on(DeployFailed)
    def failed_page(self, error: DeployFailed) -> None:
        self.pop_screen()
        self.push_screen(Failed(error.error, error.code))

    @on(Button.Pressed, "#deploy,#adopt,#folder,#wifi_conf,#openwrt,#wemos,#initialize,#new_system_template,#upgrade,#web_starter,#ap_configurator")
    def action_deployment_screen(self, event: Button.Pressed) -> None:
        screen_factory = self.SCREENS.get(event.button.id)
        if screen_factory:
            self.push_screen(screen_factory())

    @on(Button.Pressed, "#advanced")
    def action_remove_Basic_menu_and_add_Advanced(self) -> None:
        new_advanced_menu = AdvancedMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_advanced_menu)

    @on(Button.Pressed, "#back")
    def action_remove_menu_and_add_Basic(self) -> None:
        new_basic_menu = BasicMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_basic_menu)

    @on(Button.Pressed, "#wifi")
    def action_remove_Basic_menu_and_add_wifi(self) -> None:
        new_wifi_menu = WifiMenu()
        self.query_one("#right_panel").remove_children()
        self.query_one("#right_panel").mount(new_wifi_menu)

    @on(Button.Pressed, "#pop")
    def pop_screen_new(self) -> None:
        # NB: WebStarter ekraan peatab sündmuse ise (event.stop()),
        # et vältida topelt-poppi. Siin popime ainult siis, kui sündmus siia jõuab.
        self.pop_screen()

    @on(Button.Pressed, "#pop2")
    def pop_screen_new_2(self) -> None:
        self.pop_screen()
        self.pop_screen()

    @on(Button.Pressed, "#exit")
    async def exit_the_app(self) -> None:
        # Rakendusest väljumisel peatame backend'i
        await self.stop_backend()
        self.exit()

    @on(Button.Pressed, "#openwrt_submit")
    def open_wrt_logic(self) -> None:
        info = router_ip(self.current_path)
        self.pop_screen()
        self.push_screen(OpenWrtRouterIp(info, self.current_path))

    # ---------------------------
    # File tree / navigation
    # ---------------------------
    def on_mount(self) -> None:
        # load historical logs before the user opens the WebStarter screen
        self.load_log_history(max_lines=1000)

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
        """Load the given directory."""
        self.current_path = path
        self.path_display.update(f"Current Path: {self.current_path}")
        self.dir_tree.path = self.current_path
        self.dir_tree.reload()


if __name__ == "__main__":
    import sys
    start = sys.argv[1] if len(sys.argv) > 1 else None
    IotMenu(start_path=start).run()
