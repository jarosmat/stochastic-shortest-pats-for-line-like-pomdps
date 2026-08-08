from blessed import Terminal
from enum import Enum
from pypomdp import prob, belief, LineLikePomdp

from .game_results import GameResult
from .screen_result import ScreenResult
from .game_type import GameType


class POMDPGameState(Enum):
    RUNNING = 0
    WON = 1


class POMDPGameScreen:
    CONTAINER_WIDTH = 35
    CONTAINER_CHAR = "█"
    EMPTY_CHAR = " "
    WIN_THRESHOLD = 0.95

    def __init__(self, term: Terminal, validator):
        self.max_moves = 0
        self.pomdp: LineLikePomdp = LineLikePomdp([])
        self.states = 0
        self.term = term
        self.belief: belief = belief(0)
        self.input_buffer = ""
        self.game_state = POMDPGameState.RUNNING
        self.validator = validator
        self.actions = []

    def set_config(self, move_count: int, probs: list[prob]):
        self.max_moves = move_count
        self.states = len(probs) + 1
        self.pomdp = LineLikePomdp(probs)
        self.reset_game()

    def reset_game(self):
        self.belief = belief([prob(0.0) for _ in range(self.states)])
        self.belief[0] = prob(1.0)
        self.input_buffer = ""

    def _apply_action_by_index(self, action_idx: int):
        self.belief = self.pomdp.update_belief(self.belief, action_idx)
        self.actions.append(action_idx)

    def _apply_action_from_input(self):
        if not self.input_buffer:
            return
        try:
            action_idx = int(self.input_buffer)
        except ValueError:
            return
        self._apply_action_by_index(action_idx)
        self.input_buffer = ""

    def _draw_container(self, probability: prob):
        fill_count = int(float(probability) * self.CONTAINER_WIDTH)
        fill_str = self.CONTAINER_CHAR * fill_count
        empty_str = self.EMPTY_CHAR * (self.CONTAINER_WIDTH - fill_count)

        content = self.term.blue(fill_str + empty_str)

        return f"[{content}]"

    def _draw_header(self):
        print(self.term.clear())
        print(self.term.center(self.term.bold("POMDP LINE GAME")))
        print()
        print(self.term.center("States (containers with belief mass):"))
        print()

    def _draw_containers(self):
        containers = []
        labels = []
        for i in range(self.states):
            containers.append(self._draw_container(self.belief[i]))
            labels.append(f"[{i}]")

        print(self.term.center(" ".join(containers)))
        print()

    def _draw_belief(self):
        print(self.term.center("Belief:"))
        belief_strs = []
        for i, probability in enumerate(self.belief):
            prob_percent = f"{float(probability) * 100:.1f}%"
            belief_strs.append(f"S{i}={prob_percent}")
        print(self.term.center("  ".join(belief_strs)))
        print()

    def draw(self):
        self._draw_header()
        self._draw_containers()
        self._draw_belief()

        if self.game_state == POMDPGameState.WON:
            print(self.term.center(self.term.green(self.term.bold("YOU WON!"))))
        else:
            print(self.term.center(
                f"Actions taken: {self.actions}, Actions available: [{self.max_moves - len(self.actions)}/{self.max_moves}] ,Possible actions: [0-{self.states - 2}]"))

        print()

        action = f'[{self.input_buffer}]'
        print(self.term.center(action))

        print()
        print(self.term.center("I: Input action, R: Reset, Q: Quit"))

        key = self.term.inkey(timeout=0.1)

        if key.name == "KEY_ENTER" or key.code == 10:
            self._apply_action_from_input()
        elif key.lower() == "r":
            self.reset_game()
            return self._get_incomplete()
        elif key.lower() == "q":
            return self._get_exit()
        elif key.isdigit():
            self.input_buffer += key
            return self._get_incomplete()

        return None

    @staticmethod
    def _get_incomplete():
        return ScreenResult.INCOMPLETE, {}

    @staticmethod
    def _get_exit():
        return ScreenResult.EXIT, {}

    def _get_complete(self):
        return ScreenResult.COMPLETE, self._get_result()

    def _handle_enter(self):
        self._apply_action_from_input()

        if self.validator(self):
            return self._get_complete()

        return self._get_incomplete()

    def _get_result(self):
        return GameResult(GameType.MAX_PROB_T_TRIES, self.pomdp, self.belief, self.actions)
