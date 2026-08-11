from blessed import Terminal
from pypomdp import prob
from typing import Literal

from screens.number_selection_screen import NumberSelectionScreen, GameConfig
from screens.pomdp_game_screen import POMDPGameScreen
from screens.screen_validators import validate_max_prob_t_tries_win, validate_min_threshold_at_end
from screens.game_more_info import max_prob_t_tries_more_info, min_moves_before_threshold_more_info
from screens.complete_drawers import max_prob_t_tries_complete_drawer, min_threshold_complete_drawer
from screens.screen_result import ScreenResult
from screens.additional_fields import MAX_PROB_T_TRIES_NUM_MOVES_SPEC, MOVE_THRESHOLD_SPEC
from screens.intro_screen import IntroScreen
from screens.game_type import GameType


def dict_from_max_t_tries_more_info(more_info: list):
    res = dict()

    res["max_moves"] = more_info[0]

    return res


def dict_from_min_moves_bef_threshold(more_info: list):
    res = dict()

    res["threshold"] = prob(more_info[0])

    return res


max_t_tries_game_msg = ["The goal is to move as much matter to the final container",
                        "Input number of moves, number of states and move probabilities"]

min_moves_before_threshold = [
    "The goal is to move certain percentage of matter to final container in as few moves as possible",
    "Input the percentage to move, number of states and move probabilities"]


def get_game_screen(term: Terminal, type: Literal[GameType], conf: GameConfig):
    res = POMDPGameScreen(term)
    if type == GameType.MAX_PROB_T_TRIES:
        res.set_config(dict_from_max_t_tries_more_info(conf.additional_info), conf.move_probs,
                       validate_max_prob_t_tries_win, max_prob_t_tries_more_info, max_prob_t_tries_complete_drawer)
    elif type == GameType.MIN_MOVES_BEFORE_THRESHOLD:
        res.set_config(dict_from_min_moves_bef_threshold(conf.additional_info), conf.move_probs,
                       validate_min_threshold_at_end, min_moves_before_threshold_more_info,
                       min_threshold_complete_drawer)
    else:
        return None

    return res


def get_num_selection_screen(term: Terminal, type: Literal[GameType]):
    if type == GameType.MAX_PROB_T_TRIES:
        return NumberSelectionScreen(term, [MAX_PROB_T_TRIES_NUM_MOVES_SPEC], min_moves_before_threshold)
    if type == GameType.MIN_MOVES_BEFORE_THRESHOLD:
        return NumberSelectionScreen(term, [MOVE_THRESHOLD_SPEC], min_moves_before_threshold)
    return None


def main():
    term = Terminal()
    intro = IntroScreen(term)

    current_screen = intro
    game_type = GameType.MAX_PROB_T_TRIES

    with term.fullscreen(), term.cbreak(), term.hidden_cursor():
        print(term.clear())

        while True:
            msg, res = current_screen.draw()

            if msg == ScreenResult.EXIT:
                break

            if isinstance(current_screen, IntroScreen):
                if msg == ScreenResult.COMPLETE:
                    num_screen = get_num_selection_screen(term, res)
                    current_screen = num_screen if num_screen is not None else current_screen
                    game_type = res
            elif isinstance(current_screen, NumberSelectionScreen):
                if msg == ScreenResult.COMPLETE:
                    gm_screen = get_game_screen(term, game_type, res)
                    current_screen = gm_screen if gm_screen is not None else current_screen


if __name__ == "__main__":
    main()
