from .base import ScreenBase
from blessed import Terminal
from specs.base_spec import BaseGameSpec


class IntroScreen(ScreenBase):

    def __init__(self, term: Terminal, specs: list[BaseGameSpec]):
        super().__init__(term)
        self.current_field = 0
        self.specs = specs

    def _get_result(self) -> BaseGameSpec:
        return self.specs[self.current_field]

    def _draw_header(self):
        print(self.term.clear())
        self._write_line_center("Welcome to POMDP game")

    def _draw_menu(self):
        self._write_line_center("Select game type:")
        self._write_line_center("Use arrow keys to select, Enter to confirm, Q to quit")
        for i, spec in enumerate(self.specs):
            desc = spec.intro_game_description()
            if i == self.current_field:
                self._write_line_center(f"  {self.term.reverse('->')} {desc}", False)
            else:
                self._write_line_center(f"  {desc}", False)

    def _get_field_count(self) -> int:
        return len(self.specs)

    def _move_field(self, move_count: int):
        self.current_field = (self.current_field + move_count) % self._get_field_count()

    def draw(self):
        self._draw_header()
        self._draw_menu()
        self._draw_error()

        key = self.term.inkey(timeout=0.1)

        if key is None:
            return self._get_incomplete()

        if key.lower() == "q":
            return self._get_exit()

        if key.name == "KEY_ENTER" or key.code == 10:
            return self._get_complete()

        if key.name == "KEY_DOWN":
            self._move_field(1)
            return self._get_incomplete()

        if key.name == "KEY_UP":
            self._move_field(-1)
            return self._get_incomplete()

        return self._get_incomplete()
