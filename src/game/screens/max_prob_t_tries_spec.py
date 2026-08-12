from typing import TypeVar
from collections.abc import Callable

from .base_spec import BaseGameSpec
from .utils import display_percentage

T = TypeVar('T')


class MaxProbTTriesSpec(BaseGameSpec):
    DEF_MOVE_COUNT = 10
    MAX_MOVE_COUNT = 100

    def __init__(self):
        self.tries_count = self.DEF_MOVE_COUNT

    def intro_game_description(self) -> str:
        return "Max matter in final container after T moves"

    def select_additional_fields_specs(self) -> list[tuple[str, T, Callable[[str], T]]]:
        def converter(val: str):
            conv = int(val)
            if 0 < conv < self.MAX_MOVE_COUNT:
                return conv
            raise ValueError

        return [("Number of moves", self.DEF_MOVE_COUNT, converter)]

    def select_write_game_msg(self, screen):
        msg = ["The goal is to move as much matter to the final container",
               "Input number of moves, number of states and move probabilities"]

        for mes in msg:
            print(screen.term.center(mes))
            print()

    def game_validate(self, screen) -> bool:
        return screen.actions_taken_count() == self.tries_count

    def game_write_more_info(self, screen):
        print(screen.term.center(
            f"Actions available: [{self.tries_count - screen.actions_taken_count()}/{self.tries_count}]"))
        print()

    def game_write_completed_info(self, screen):
        print(screen.term.center("Game Complete!"))
        print()
        res = screen.pomdp.prob_at_end(screen.belief)
        print(screen.term.center(f"You managed to move {display_percentage(res)} of matter to end state"))
        print()
        print(screen.term.center("It is possible to move {some amount}"))
        print()

    def set_more_info(self, info_vals: list):
        if len(info_vals) != 0 and isinstance(info_vals[0], int):
            self.tries_count = info_vals[0]
