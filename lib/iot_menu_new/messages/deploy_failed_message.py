from textual.message import Message


class DeployFailed(Message):
    def __init__(self, error: str, code: int):
        self.error = error
        self.code = code
        super().__init__()

