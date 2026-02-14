from textual.app import App, ComposeResult
from textual.widgets import Static, ListView, ListItem, Label, Button
from textual.reactive import reactive
from textual.screen import Screen
from script_activation_logic.adopt_script import run_adopt_scan, adopt_node
from pathlib import Path


class AdoptScreen(Screen):
    nodes = reactive([])
    saved_output = reactive([])
    start_index = reactive(0)
    max_entries = 5
    def __init__(self, current_path: str = None, **kwargs):
            super().__init__(**kwargs)
            self.current_path = Path(current_path or Path.cwd())
    def compose(self) -> ComposeResult:
        yield Static("Adoption tool", id="title")
        yield Static("", id="status")
        yield Button("Go back", id="pop")
        yield ListView(id="menu")

    def on_mount(self) -> None:
        self.call_later(self.adopt)

    def adopt(self):
        status = self.query_one("#status", Static)
        status.update("\nScanning for adoptable nodes...")

        try:
            ap_list, saved_output = run_adopt_scan(self.current)
        except:
            ap_list=[]
            saved_output=[]
        self.nodes = ap_list
        self.saved_output = saved_output

        if ap_list:
            status.update("ok\nSelect a node to adopt:")
            self.show_page(0)
        else:
            status.update(
                "\n[red]No node to adopt found[/red]\n"
                "Make sure you have at least one node in adoption mode.\n\n"
                "Press Button to quit."
            )

    def show_page(self, start: int):
        """Show 5 nodes at a time + More/Previous/Back buttons."""
        menu = self.query_one("#menu", ListView)
        menu.clear()
        self.start_index = start

        counter = 1
        for idx in range(start, min(start + self.max_entries, len(self.nodes))):
            label = f"{self.nodes[idx]} ({counter})"
            menu.append(ListItem(Label(label)))
            counter += 1

        # add More
        if start + self.max_entries < len(self.nodes):
            menu.append(ListItem(Label("More nodes (M)")))
        # lisa Previous
        if start > 0:
            menu.append(ListItem(Label("Previous nodes (P)")))
        else:
            menu.append(ListItem(Label("Back (B,X)")))

    def on_list_view_selected(self, event: ListView.Selected) -> None:
        label = event.item.query_one(Label).renderable
        text = str(label)

        if text.startswith("node "):
            selection = text.split(" ")[1]  # take node id
            adopt_node(selection)
            self.app.pop_screen()
        elif text.startswith("More"):
            self.show_page(self.start_index + self.max_entries)
        elif text.startswith("Previous"):
            self.show_page(max(self.start_index - self.max_entries, 0))
        elif text.startswith("Back"):
            self.app.pop_screen()