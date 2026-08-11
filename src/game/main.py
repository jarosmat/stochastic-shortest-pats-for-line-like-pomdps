from blessed import Terminal
from pypomdp import prob

from screens.number_selection_screen import NumberSelectionScreen
from screens.pomdp_game_screen import POMDPGameScreen
from screens.screen_validators import validate_max_prob_t_tries_win, validate_min_threshold_at_end
from screens.game_more_info import max_prob_t_tries_more_info, min_moves_before_threshold_more_info
from screens.complete_drawers import max_prob_t_tries_complete_drawer, min_threshold_complete_drawer
from screens.screen_result import ScreenResult
from screens.additional_fields import MAX_PROB_T_TRIES_NUM_MOVES_SPEC, MOVE_THRESHOLD_SPEC


def dict_from_max_t_tries_more_info(more_info: list):
    res = dict()

    res["max_moves"] = more_info[0]

    return res

def dict_from_min_moves_bef_threshold(more_info: list):
    res = dict()

    res["threshold"] = prob(more_info[0])

    return res

max_t_tries_game_msg =  ["The goal is to move as much matter to the final container",
                         "Input number of moves, number of states and move probabilities"]

min_moves_before_threshold = ["The goal is to move certain percentage of matter to final container in as few moves as possible",
                             "Input the percentage to move, number of states and move probabilities"]

def main():
    term = Terminal()

    selection_screen = NumberSelectionScreen(term, [MAX_PROB_T_TRIES_NUM_MOVES_SPEC],
                                             max_t_tries_game_msg)

    game_screen = POMDPGameScreen(term)
    current_screen = selection_screen

    with term.fullscreen(), term.cbreak(), term.hidden_cursor():
        print(term.clear())

        while True:

            msg, res = current_screen.draw()

            if msg == ScreenResult.EXIT:
                break

            if current_screen == selection_screen:
                if msg == ScreenResult.COMPLETE:
                    current_screen = game_screen
                    game_screen.set_config(dict_from_max_t_tries_more_info(res.additional_info), res.move_probs,
                                           validate_max_prob_t_tries_win,
                                           max_prob_t_tries_more_info, max_prob_t_tries_complete_drawer)


if __name__ == "__main__":
    main()
