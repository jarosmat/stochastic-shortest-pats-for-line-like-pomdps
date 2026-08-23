from blessed import Terminal
from typing import Literal

from screens.number_selection_screen import NumberSelectionScreen
from screens.pomdp_game_screen import POMDPGameScreen
from screens.intro_screen import IntroScreen
from specs.max_prob_t_tries_spec import MaxProbTTriesSpec
from specs.min_moves_before_threshold_spec import MinMovesBeforeThresholdSpec
from screens.screen_result import ScreenResult


def main():
    specs = [MaxProbTTriesSpec(), MinMovesBeforeThresholdSpec()]
    term = Terminal()
    intro = IntroScreen(term, specs)
    game = POMDPGameScreen(term)

    current_screen = intro

    with term.fullscreen(), term.cbreak(), term.hidden_cursor():
        print(term.clear())

        while True:
            msg, res = current_screen.draw()

            if msg == ScreenResult.EXIT:
                break

            if msg == ScreenResult.COMPLETE:
                if current_screen == intro:
                    current_screen = NumberSelectionScreen(term, res)
                else:
                    game.set_config(res)
                    current_screen = game


if __name__ == "__main__":
    main()
