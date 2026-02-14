# screens/web_starter_screen.py
from __future__ import annotations

import asyncio
from textual.app import ComposeResult
from textual.containers import Vertical
from textual.widgets import Label, Button, Log
from textual.screen import Screen
from textual import on

from messages.deploy_failed_message import DeployFailed
from messages.web_output import WebOutput


class WebStarter(Screen):
    """Screen that shows the web starter status and log.

    The process itself runs at the App level. The screen may be mounted
    and unmounted independently.
    """

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.status_label = Label("Status: Stopped", id="status")
        self.log_widget = Log(id="log")       
        self._popping: bool = False
        self._stopping: bool = False  


    def compose(self) -> ComposeResult:
        yield Vertical(
            Label("You're about to start the web kit in IoT"),
            self.status_label,
            Button("Start web starter", id="start"),
            Button("Stop web starter", id="stop"),
            Button("Go back", id="pop"),
            self.log_widget,
        )

    async def on_mount(self) -> None:
        """Replay the buffer and show current status when the screen opens."""
        app = self.app
        running = app.web_starter is not None and app.web_starter.returncode is None
        self.status_label.update("Status: Running ✅" if running else "Status: Stopped ❌")

        # Taasesita olemasolev buffer
        if app.web_log_buffer:
            self.log_widget.clear()
            for line in app.web_log_buffer:
                self.log_widget.write_line(line)

    async def on_button_pressed(self, event: Button.Pressed):
        app = self.app

        if event.button.id == "start":
            try:
                await app.start_web_starter()
                # Update status
                running = app.web_starter is not None and app.web_starter.returncode is None
                self.status_label.update("Status: Running ✅" if running else "Status: Stopped ❌")

                # Logi teade
                if running and app.web_starter:
                    self.log_widget.write_line(f"Web starter launched (PID={app.web_starter.pid})")
                else:
                    self.log_widget.write_line("Web starter did not start.")
            except Exception:
                # If startup failed (e.g., not in IoTempower environment)
                self.post_message(DeployFailed("You're not in IoT", 4))
            finally:
                event.stop()  # Stop event from bubbling to the App level

        
        elif event.button.id == "stop":
            # avoid repeated stop calls
            if self._stopping:
                event.stop(); return
            self._stopping = True
            # Stop should not race with the log/screen; it only stops the backend
            try:
                await app.stop_backend()
                self.status_label.update("Status: Stopped ❌")
                # (stoppimise logirida tuleb App-ist WebOutputi kaudu)
            finally:
                self._stopping = False
            event.stop()


        
        elif event.button.id == "pop":
            # --- DEBOUNCE / LOCK: only 1 pop ---
            if self._popping:
                event.stop(); return
            self._popping = True
            # disable the button to prevent double-clicks
            try:
                self.query_one("#pop", Button).disabled = True
            except Exception:
                pass
            
            try:
                self.app.pop_screen()
            except Exception:
                # ignore if there's nothing left to pop on the stack
                pass
            finally:
                event.stop()



    @on(WebOutput)
    def _on_web_output(self, msg: WebOutput) -> None:
        """Append incoming log line to the log widget if the screen is active."""
        if self.is_mounted:
            self.log_widget.write_line(msg.line)