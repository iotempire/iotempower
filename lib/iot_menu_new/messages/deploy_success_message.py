from textual.message import Message


class DeploySuccess(Message):
    def __init__(self, message: str):
        self.message = message
        super().__init__()


