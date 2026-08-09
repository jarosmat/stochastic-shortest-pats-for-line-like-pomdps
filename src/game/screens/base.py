from abc import ABC, abstractmethod
from blessed import Terminal
from .screen_result import ScreenResult


class ScreenBase(ABC):
    def __init__(self, term: Terminal):
        self.term = term
        self.input_buffer = ""
        self.err_msg = ""

    @abstractmethod
    def draw(self):
        pass

    @abstractmethod
    def _get_result(self):
        pass

    def _delete_char(self):
        if self.input_buffer != '':
            self.input_buffer = self.input_buffer[:-1]

    def _draw_error(self):
        if self.err_msg != "":
            print(self.term.center(self.term.bold_red(self.err_msg)))
            print()

    @staticmethod
    def _get_incomplete():
        return ScreenResult.INCOMPLETE, {}

    @staticmethod
    def _get_exit():
        return ScreenResult.EXIT, {}

    def _get_complete(self):
        return ScreenResult.COMPLETE, self._get_result()

    @staticmethod
    def _display_value(value):
        if value is None:
            return ""
        return str(value)
