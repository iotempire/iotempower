from pathlib import Path

from textual import on
from textual.app import ComposeResult
from textual.binding import Binding
from textual.containers import Vertical
from textual.screen import Screen
from textual.widgets import Button, Footer, Header, Static, TextArea


class FileEditorScreen(Screen):
    BINDINGS = [
        Binding("escape", "close_editor", "Close"),
    ]

    def __init__(self, file_path: Path, **kwargs):
        super().__init__(**kwargs)
        self.file_path = Path(file_path)
        self.editor = TextArea(id="file_editor")
        self.status = Static("", id="editor_status")

    def compose(self) -> ComposeResult:
        yield Header()
        yield Static(f"Editing: {self.file_path}", id="editor_path")
        with Vertical():
            yield self.editor
            yield self.status
            yield Button("Save", id="save_file")
            yield Button("Close", id="close_editor")
        yield Footer()

    def on_mount(self) -> None:
        try:
            content = self.file_path.read_text(encoding="utf-8", errors="replace")
            self.editor.load_text(content)
            self.editor.focus()
        except OSError as error:
            self.status.update(f"[red]Could not open file: {error}[/red]")

    @on(Button.Pressed, "#save_file")
    def save_file(self) -> None:
        try:
            self.file_path.write_text(
                self.editor.text,
                encoding="utf-8",
                newline="",
            )
            self.status.update("[green]Saved[/green]")
        except OSError as error:
            self.status.update(f"[red]Could not save file: {error}[/red]")

    @on(Button.Pressed, "#close_editor")
    def close_button(self) -> None:
        self.app.pop_screen()

    def action_close_editor(self) -> None:
        self.app.pop_screen()
