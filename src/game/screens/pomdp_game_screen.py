from multiprocessing import Process, Event, get_context
from blessed import Terminal
from enum import Enum
from pypomdp import prob, belief, LineLikePomdp

from .base import ScreenBase
from .utils import display_percentage
from .number_selection_screen import GameConfig


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
        self.pomdp: LineLikePomdp = LineLikePomdp([])
        self.states: int = 0
        self.belief: belief = belief(0)
        self.game_state = POMDPGameState.RUNNING
        self.actions: list[int] = []
        self.spec = None
        self.compute_process = None
        self.stop_event = get_context('spawn').Event()
        self.move_probs_floats: list[float] = []

    def set_config(self, conf: GameConfig):
        self._stop_computation()
        self.spec = conf.spec
        self.states = len(conf.move_probs) + 1
        self.pomdp = LineLikePomdp(conf.move_probs)
        self.move_probs_floats = [float(p) for p in conf.move_probs]
        self.reset_game()
        self._start_computation()

    def _start_computation(self):
        if self.spec is None:
            return
        self.stop_event.clear()
        self.compute_process = Process(
            target=self.spec.calculate_best_res,
            args=(self.stop_event, self.move_probs_floats)
        )
        self.compute_process.start()

    def _stop_computation(self):
        self.stop_event.set()
        if self.compute_process and self.compute_process.is_alive():
            self.compute_process.join(timeout=0.5)
            if self.compute_process.is_alive():
                self.compute_process.terminate()
        self.compute_process = None

    def reset_game(self):
        self.belief = belief([prob(0.0) for _ in range(self.states)])
        self.belief[0] = prob(1.0)
        self.states = len(self.belief)
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
            self._clear_error_msg()
        except RuntimeError:
            self._set_error_msg(f"Invalid Action: {action_idx}")

        if self.spec.game_validate(self):
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
        self._clear_term()
        self._write_line_center(self.term.bold("POMDP LINE GAME"))

    def _draw_containers(self):
        self._write_line_center("States (containers with belief mass):")
        containers = []
        labels = []
        for i in range(self.states):
            containers.append(self._draw_container(self.belief[i]))
            labels.append(f"[{i}]")

        self._write_line_center(" ".join(containers))

    def _draw_belief(self):
        self._write_line_center("Belief:", False)
        belief_strs = []
        for i in range(self.states):
            prob_percent = display_percentage(self.belief[i])
            belief_strs.append(f"S{i}={prob_percent}")
        self._write_line_center("  ".join(belief_strs))

    def _draw_info(self):
        self._write_line_center(
            f"Actions taken: {self.actions}, Possible actions: [0-{self.states - 2}]")

    def _draw_input_buffer(self):
        action = f'[{self.input_buffer}]'
        self._write_line_center(action)

    def _draw_controls(self):
        self._write_line_center("R: Reset, Q: Quit")

    def _draw_more_info(self):
        if self.spec is not None:
            self.spec.game_write_more_info(self)

    def _draw_complete_msg(self):
        if self.spec is not None:
            self.spec.game_write_completed_info(self)

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
            return self._get_incomplete()

        if key.name == "KEY_BACKSPACE" or key.code == 127:
            self._delete_char()
            return self._get_incomplete()

        if key.lower() == "r":
            self.reset_game()
            return self._get_incomplete()

        if key.lower() == "q":
            self._stop_computation()
            return self._get_exit()

        if key.isdigit():
            self.input_buffer += key
            return self._get_incomplete()

        return self._get_incomplete()

    def _get_result(self):
        return None

    def actions_taken_count(self) -> int:
        return len(self.actions)

    def _delete_char(self):
        if self.input_buffer != '':
            self.input_buffer = self.input_buffer[:-1]
