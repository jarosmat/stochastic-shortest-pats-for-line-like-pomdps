from blessed import Terminal

from screens.number_selection_screen import NumberSelectionScreen
from screens.pomdp_game_screen import POMDPGameScreen
from pypomdp import prob
from screens.screen_validators import validate_max_prob_t_tries_win
from screens.game_more_info import max_prob_t_tries_more_info
from screens.complete_drawers import max_prob_t_tries_complete_drawer
from screens.screen_result import ScreenResult
from screens.additional_fields import MAX_PROB_T_TRIES_NUM_MOVES_SPEC


def main():
    term = Terminal()

    selection_screen = NumberSelectionScreen(term, [MAX_PROB_T_TRIES_NUM_MOVES_SPEC],
                                             ["The goal is to move as much matter to the final container",
                                              "Input number of moves, number of states and move probabilities"])
    game_screen = POMDPGameScreen(term)
    game_screen.set_config(10, [prob(0.5), prob(0.5)], validate_max_prob_t_tries_win, max_prob_t_tries_more_info,
                           max_prob_t_tries_complete_drawer)
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
                    game_screen.set_config(res.moves_count, res.move_probs, validate_max_prob_t_tries_win,
                                           max_prob_t_tries_more_info, max_prob_t_tries_complete_drawer)


if __name__ == "__main__":
    main()
