from blessed import Terminal
from enum import Enum
from pypomdp import prob, belief, LineLikePomdp

from .game_results import GameResult
from .game_type import GameType
from .base import ScreenBase
from .utils import display_percentage


class POMDPGameState(Enum):
    RUNNING = 0
    COMPLETE = 1


class POMDPGameScreen(ScreenBase):
    CONTAINER_WIDTH = 35
    CONTAINER_CHAR = "█"
    EMPTY_CHAR = " "
    WIN_THRESHOLD = 0.95

    def __init__(self, term: Terminal):
        super().__init__(term)
        self.max_moves: int = 0
        self.pomdp: LineLikePomdp = LineLikePomdp([])
        self.states: int = 0
        self.belief: belief = belief(0)
        self.game_state = POMDPGameState.RUNNING
        self.validator = None
        self.actions: list[int] = []
        self.more_info = None
        self.complete_drawer = None

    def set_config(self, move_count: int, probs: list[prob], validator, more_info, complete_drawer):
        self.max_moves = move_count
        self.states = len(probs) + 1
        self.pomdp = LineLikePomdp(probs)
        self.reset_game()
        self.validator = validator
        self.more_info = more_info
        self.complete_drawer = complete_drawer

    def reset_game(self):
        self.belief = belief([prob(0.0) for _ in range(self.states)])
        self.belief[0] = prob(1.0)
        self.actions = []
        self.input_buffer = ""
        self.game_state = POMDPGameState.RUNNING

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
        try:
            self._apply_action_by_index(action_idx)
        except RuntimeError:
            self.err_msg = f"Invalid Action: {action_idx}"

        if callable(self.validator) and self.validator(self):
            self.game_state = POMDPGameState.COMPLETE

        self.input_buffer = ""

    def _get_fill_count(self, probability: prob):
        fill_count = int(float(probability) * self.CONTAINER_WIDTH)

        if fill_count == 0 and probability > prob(0.0):
            return 1

        return fill_count

    def _draw_container(self, probability: prob):
        fill_count = self._get_fill_count(probability)
        fill_str = self.CONTAINER_CHAR * fill_count
        empty_str = self.EMPTY_CHAR * (self.CONTAINER_WIDTH - fill_count)

        content = self.term.blue(fill_str + empty_str)

        return f"[{content}]"

    def _draw_header(self):
        print(self.term.clear())
        print(self.term.center(self.term.bold("POMDP LINE GAME")))
        print()

    def _draw_containers(self):
        print(self.term.center("States (containers with belief mass):"))
        print()
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
        for i in range(self.states):
            prob_percent = display_percentage(self.belief[i])
            belief_strs.append(f"S{i}={prob_percent}")
        print(self.term.center("  ".join(belief_strs)))
        print()

    def _draw_info(self):
        print(self.term.center(
            f"Actions taken: {self.actions}, Possible actions: [0-{self.states - 2}]"))
        print()

    def _draw_input_buffer(self):
        action = f'[{self.input_buffer}]'
        print(self.term.center(action))
        print()

    def _draw_controls(self):
        print(self.term.center("R: Reset, Q: Quit"))
        print()

    def _draw_more_info(self):
        if callable(self.more_info):
            self.more_info(self)
            print()

    def _draw_complete_msg(self):
        if callable(self.complete_drawer):
            self.complete_drawer(self)

    def _draw_running(self):
        self._draw_header()
        self._draw_containers()
        self._draw_belief()
        self._draw_info()
        self._draw_more_info()
        self._draw_input_buffer()
        self._draw_controls()
        self._draw_error()

    def _draw_complete(self):
        self._draw_header()
        self._draw_complete_msg()
        self._draw_containers()
        self._draw_belief()
        self._draw_info()
        self._draw_controls()

    def draw(self):
        if self.game_state == POMDPGameState.RUNNING:
            self._draw_running()
        elif self.game_state == POMDPGameState.COMPLETE:
            self._draw_complete()

        key = self.term.inkey(timeout=0.1)

        if key.name == "KEY_ENTER" or key.code == 10:
            self._apply_action_from_input()
            return self._get_complete()
        elif key.lower() == "r":
            self.reset_game()
            return self._get_incomplete()
        elif key.lower() == "q":
            return self._get_exit()
        elif key.isdigit():
            self.input_buffer += key
            return self._get_incomplete()

        return None

    def _handle_enter(self):
        self._apply_action_from_input()

        if callable(self.validator) and self.validator(self):
            return self._get_complete()

        return self._get_incomplete()

    def _get_result(self):
        return GameResult(GameType.MAX_PROB_T_TRIES, self.pomdp, self.belief, self.actions)
