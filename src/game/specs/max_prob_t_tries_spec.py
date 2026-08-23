from typing import TypeVar
from collections.abc import Callable
from multiprocessing import Event
from pypomdp import LineLikePomdp, prob, belief

from .base_spec import BaseGameSpec
from utils.utils import display_percentage

T = TypeVar('T')


class MaxProbTTriesSpec(BaseGameSpec):
    DEF_MOVE_COUNT = 10
    MAX_MOVE_COUNT = 100

    def __init__(self):
        super().__init__()
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

    def select_write_game_msg(self) -> list[str]:
        msg = ["The goal is to move as much matter to the final container",
               "Input number of moves, number of states and move probabilities"]

        return msg

    def game_validate(self, pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> bool:
        return len(moves) == self.tries_count

    def game_write_more_info(self, pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> list[str]:
        actions_count = len(moves)
        more_info = [f"Actions available: [{self.tries_count - actions_count}/{self.tries_count}]"]

        return more_info

    def game_write_completed_info(self, pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> list[str]:
        info = ["Game Complete!"]
        res = pomdp.prob_at_end(bel)
        info.append(f"You managed to move {display_percentage(res)} of matter to end state")
        moves = self._get_best_moves()
        if moves is None:
            info.append("Waiting for computation of best moves")
        else:
            pol, per = moves
            info.append(f"It is possible to move {display_percentage(prob(per))}")
            info.append(f"Using moves: {pol}")

        return info

    def set_more_info(self, info_vals: list):
        if len(info_vals) != 0 and isinstance(info_vals[0], int):
            self.tries_count = info_vals[0]

    def calculate_best_res(self, stop_event: Event, move_probs: list[float]):
        from pypomdp import prob, LineLikePomdp, max_prob_t_tries_solve, belief
        try:
            pomdp = LineLikePomdp([prob(p) for p in move_probs])
            bel_len = len(move_probs) + 1
            bel_l = [prob(0) for _ in range(bel_len)]
            if bel_len > 0:
                bel_l[0] = prob(1)
            start_bel = belief(bel_l)
            result = max_prob_t_tries_solve(pomdp, self.tries_count, start_bel)
            pol, bel = result
            pol.reset()
            last = 0
            res_moves = []
            for move in pol:
                if move < last:
                    break
                last = move
                res_moves.append(move)

            self.result_queue.put((res_moves, float(bel)))
        except Exception:
            pass
