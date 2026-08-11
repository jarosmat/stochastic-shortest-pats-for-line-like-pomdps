from .base import ScreenBase
from blessed import Terminal
from .game_type import GameType


class IntroScreen(ScreenBase):

    def __init__(self, term: Terminal):
        super().__init__(term)
        self.current_field = 0
        self.game_types = list(GameType)

    def _get_result(self):
        return self.game_types[self.current_field]

    def _draw_header(self):
        print(self.term.clear())
        print(self.term.center("Welcome to POMDP game"))
        print()

    def _draw_menu(self):
        print(self.term.center("Select game type:"))
        print()
        for i, game_type in enumerate(self.game_types):
            if i == self.current_field:
                print(self.term.center(f"  {self.term.reverse('->')} {game_type.name.replace('_', ' ').title()}"))
            else:
                print(self.term.center(f"  {game_type.name.replace('_', ' ').title()}"))
        print()
        print(self.term.center("Use arrow keys to select, Enter to confirm, Q to quit"))


    def _get_field_count(self):
        return len(self.game_types)

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
