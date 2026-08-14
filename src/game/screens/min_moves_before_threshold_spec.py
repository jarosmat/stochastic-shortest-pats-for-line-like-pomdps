from typing import TypeVar
from collections.abc import Callable
from pypomdp import prob, LineLikePomdp
from multiprocessing import Event

from .base_spec import BaseGameSpec
from .utils import display_percentage

T = TypeVar('T')


class MinMovesBeforeThresholdSpec(BaseGameSpec):
    DEF_THRESHOLD = 0.5
    MAX_THRESHOLD = 0.8

    def __init__(self):
        self.threshold = self.DEF_THRESHOLD

    def intro_game_description(self) -> str:
        return "Min number of moves before certain amount in final container"

    def select_additional_fields_specs(self) -> list[tuple[str, T, Callable[[str], T]]]:
        def converter(val: str):
            conv = float(val)
            if 0 < conv < self.MAX_THRESHOLD:
                return conv
            raise ValueError

        return [("Amount of matter to move", self.DEF_THRESHOLD, converter)]

    def select_write_game_msg(self, screen):
        msg = [
            "The goal is to move certain percentage of matter to final container in as few moves as possible",
            "Input the percentage to move, number of states and move probabilities"]

        for mes in msg:
            print(screen.term.center(mes))
            print()

    def game_validate(self, screen) -> bool:
        return screen.pomdp.is_at_end_prob(screen.belief, self.threshold)

    def game_write_more_info(self, screen):
        thr = self.threshold
        bel = screen.pomdp.prob_at_end(screen.belief)

        print(screen.term.center(
            f"Already moved: {display_percentage(bel)} of matter, to end the game you need to move {display_percentage(thr)} of matter"))
        print()

    def game_write_completed_info(self, screen):
        print(screen.term.center("Game Complete!"))
        print()
        mvs = screen.actions_taken_count()
        print(screen.term.center(
            f"You managed to move {display_percentage(self.threshold)} of matter to end in {mvs} moves"))
        print()
        print(screen.term.center("It is possible to do so in {some amount}"))
        print()

    def set_more_info(self, info_vals: list):
        if len(info_vals) != 0 and isinstance(info_vals[0], float):
            self.threshold = prob(info_vals[0])

    def calculate_best_res(self, stop_event: Event, pomdp: LineLikePomdp):
        pass
