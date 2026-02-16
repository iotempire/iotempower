from textual.message import Message

class WebOutput(Message):
    def __init__(self, line: str) -> None:
        super().__init__()
        self.line = line
