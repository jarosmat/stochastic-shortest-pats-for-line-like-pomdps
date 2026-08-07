from blessed import Terminal
from enum import Enum
from .screen_draw_result import ScreenDrawResult


class MaxProbTTriesConfig:
    def __init__(self, moves_count: int, move_probs: list[float]):
        self.moves_count = moves_count
        self.move_probs = move_probs


class MaxProbTTriesNumberSelectionScreen:
    DEF_STATE_COUNT = 3
    MAX_STATE_COUNT = 5
    MIN_STATE_COUNT = 3

    DEF_PROB = 0.5
    PROB_LOWER_BOUND = 0.1
    PROB_UPPER_BOUND = 1

    DEF_MOVE_COUNT = 10
    MAX_MOVE_COUNT = 100

    MOVE_COUNT_FIELD = 0
    STATE_COUNT_FIELD = 1
    PROB_FIELDS_OFFSET = 2

    class InputFieldTypes(Enum):
        INTEGER = 0
        PROBABILITY = 1

    def __init__(self, term: Terminal):
        self.term = term
        self.state_count = MaxProbTTriesNumberSelectionScreen.DEF_STATE_COUNT
        self.move_count = self.DEF_MOVE_COUNT
        self.probabilities = [MaxProbTTriesNumberSelectionScreen.DEF_PROB for _ in range(self.state_count - 1)]
        self.current_field = self.MOVE_COUNT_FIELD
        self.input_buffer = ""
        self._load_field_input()

    def _get_field_value(self, idx: int):
        if idx == self.MOVE_COUNT_FIELD:
            return self._display_value(self.move_count)
        if idx == self.STATE_COUNT_FIELD:
            return self._display_value(self.state_count)
        if idx < self._get_field_count():
            return self._display_value(self.probabilities[idx - self.PROB_FIELDS_OFFSET])

        return None

    def _draw_input_field(self, msg: str, field_id: int):
        if msg is not None and msg != '':
            print(self.term.bold(msg))
        if self.current_field == field_id:
            print(f"  [{self.term.reverse(self.input_buffer)}]")
        else:
            print(f"  [{self._display_value(self._get_field_value(field_id))}]")
        print()

    def _draw_move_selector(self):
        self._draw_input_field('number of moves', self.MOVE_COUNT_FIELD)

    def _draw_state_selector(self):
        self._draw_input_field('Number of states', self.STATE_COUNT_FIELD)

    def _draw_prob_selection(self):
        for i in range(self.state_count - 1):
            self._draw_input_field(f"Move probability {i + 1}:", i + self.PROB_FIELDS_OFFSET)

    def _draw_header(self):
        print(self.term.clear())

        print(self.term.center("Input number of moves, number of states and move probabilities"))
        print()
        print(self.term.center("Type numbers, Tab: next field, Enter: confirm field/submit, Q: quit"))
        print()

    def _get_field_count(self):
        return self.PROB_FIELDS_OFFSET + (self.state_count - 1)

    def _move_field(self, move_count: int):
        self.current_field = (self.current_field + move_count) % self._get_field_count()
        self._load_field_input()

    def _delete_char(self):
        if self.input_buffer != '':
            self.input_buffer = self.input_buffer[:-1]

    def _handle_tabs(self, move: int):
        self._save_current_input()
        self._move_field(move)

    @staticmethod
    def _get_incomplete():
        return ScreenDrawResult.INCOMPLETE, {}

    @staticmethod
    def _get_exit():
        return ScreenDrawResult.EXIT, {}

    def _get_move(self):
        return ScreenDrawResult.MOVE_NEXT, self._get_result()

    def draw(self):
        self._draw_header()
        self._draw_move_selector()
        self._draw_state_selector()
        self._draw_prob_selection()

        key = self.term.inkey(timeout=0.1)

        if key is None:
            return self._get_incomplete()

        if key.lower() == "q":
            return self._get_exit()

        if key.name == "KEY_ENTER" or key.code == 10:
            self._handle_enter()
            return self._get_move()

        if key.name == "KEY_TAB":
            self._handle_tabs(1)
            return self._get_incomplete()

        if key.name == "KEY_BTAB":
            self._handle_tabs(-1)
            return self._get_incomplete()

        if key.name == "KEY_BACKSPACE" or key.code == 127:
            self._delete_char()
            return self._get_incomplete()

        if key.name == "KEY_DC":
            self._delete_char()
            return self._get_incomplete()

        if key == ".":
            if self._validate_dot():
                self.input_buffer += key
            return self._get_incomplete()

        if key.isdigit():
            self.input_buffer += key
            return self._get_incomplete()

        return self._get_incomplete()

    def _validate_dot(self) -> bool:
        if self.current_field == self.STATE_COUNT_FIELD or self.current_field == self.MOVE_COUNT_FIELD:
            return False

        return '.' not in self.input_buffer

    @staticmethod
    def _display_value(value):
        if value is None:
            return ""
        return str(value)

    def _handle_enter(self):
        self._save_current_input()

    def _get_converter(self):
        if self.current_field == self.MOVE_COUNT_FIELD or self.current_field == self.STATE_COUNT_FIELD:
            return int
        return float

    def _update_probs(self, dsize: int):
        if dsize < 0:
            return
        pl = len(self.probabilities)
        while pl > dsize:
            self.probabilities.pop()
            pl -= 1

        while pl < dsize:
            self.probabilities.append(self.DEF_PROB)
            pl += 1

    def _set_move_count(self, value: int):
        if value < self.MAX_MOVE_COUNT:
            self.move_count = value

    def _set_state_count(self, value: int):
        if self.MIN_STATE_COUNT <= value <= self.MAX_STATE_COUNT:
            self.state_count = value
            self._update_probs(value - 1)

    def _set_prob_val(self, p_idx: int, value: float):
        if p_idx < self.state_count - 1 and self.PROB_LOWER_BOUND < value < self.PROB_UPPER_BOUND:
            self.probabilities[p_idx] = value

    def _set_value(self, value):
        if self.current_field == self.MOVE_COUNT_FIELD:
            self._set_move_count(value)
        elif self.current_field == self.STATE_COUNT_FIELD:
            self._set_state_count(value)
        else:
            self._set_prob_val(self.current_field - self.PROB_FIELDS_OFFSET, value)

    def _save_current_input(self):
        if not self.input_buffer:
            return

        converter = self._get_converter()

        try:
            value = converter(self.input_buffer)
        except ValueError:
            return

        self._set_value(value)

        self.input_buffer = ""

    def _load_field_input(self):
        if self.input_buffer != '':
            return
        if self.current_field == self.MOVE_COUNT_FIELD:
            self.input_buffer = self._display_value(self.move_count)
        elif self.current_field == self.STATE_COUNT_FIELD:
            self.input_buffer = self._display_value(self.state_count)
        else:
            p_idx = self.current_field - self.PROB_FIELDS_OFFSET
            self.input_buffer = self._display_value(self.probabilities[p_idx])

    def _get_result(self):
        return MaxProbTTriesConfig(self.move_count, self.probabilities)
