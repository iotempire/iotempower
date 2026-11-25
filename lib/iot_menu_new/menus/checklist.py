from textual import on
from textual.app import ComposeResult
from textual.containers import VerticalScroll
from textual.widgets import Button
from rich.text import Text


class Checklist(VerticalScroll):
    """Main menu whose items display whether they have been pressed."""

    ITEMS = (
        "Deploy",
        "Adopt",
        "Wifi Network Setup",
        "Web starter",
        "AP Configurator",
        "Exit",
    )

    def compose(self) -> ComposeResult:
        for label in self.ITEMS:
            button = Button(Text.from_markup(f"{label}: [red]✗ Missing[/red]"))
            button.add_class("missing")
            yield button

    @on(Button.Pressed)
    def toggle_status(self, event: Button.Pressed) -> None:
        button = event.button
        label = str(button.label).split(": ", 1)[0]

        if button.has_class("done"):
            button.remove_class("done")
            button.add_class("missing")
            button.label = Text.from_markup(f"{label}: [red]✗ Missing[/red]")
        else:
            button.remove_class("missing")
            button.add_class("done")
            button.label = Text.from_markup(f"{label}: [green]✓ Done[/green]")


# Compatibility name for code that imports BasicMenu from this module.
BasicMenu = Checklist