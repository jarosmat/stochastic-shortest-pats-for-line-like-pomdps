from collections.abc import Callable
from typing import TypeVar

from blessed import Terminal
from enum import Enum
from pypomdp import prob
from .base import ScreenBase
from .base_spec import BaseGameSpec


class GameConfig:
    def __init__(self, move_probs: list[prob], spec: BaseGameSpec):
        self.move_probs: list[prob] = move_probs
        self.spec = spec


T = TypeVar('T')


class NumberSelectionScreen(ScreenBase):
    DEF_STATE_COUNT = 3
    MAX_STATE_COUNT = 5
    MIN_STATE_COUNT = 3

    DEF_PROB = 0.5
    PROB_LOWER_BOUND = 0.1
    PROB_UPPER_BOUND = 1

    PROB_FIELDS_OFFSET = 2

    def __init__(self, term: Terminal, spec: BaseGameSpec):
        super().__init__(term)
        self.spec = spec
        self.state_count = NumberSelectionScreen.DEF_STATE_COUNT
        self.probabilities = [NumberSelectionScreen.DEF_PROB for _ in range(self.state_count - 1)]
        self.current_field = 0
        self.additional_fields = spec.select_additional_fields_specs()
        self.STATE_COUNT_FIELD = len(self.additional_fields)
        self.additional_fields_vals = [self.additional_fields[i][1] for i in range(self.STATE_COUNT_FIELD)]
        self.PROB_FIELDS_OFFSET = self.STATE_COUNT_FIELD + 1
        self._load_field_input()

    def _get_field_value(self, idx: int):
        if idx < self.STATE_COUNT_FIELD:
            return self._display_value(self.additional_fields_vals[idx])
        if idx == self.STATE_COUNT_FIELD:
            return self.state_count
        if idx < self._get_field_count():
            return self.probabilities[idx - self.PROB_FIELDS_OFFSET]

        return None

    def _draw_input_field(self, msg: str, field_id: int):
        if msg is not None and msg != '':
            print(self.term.bold(msg))
        if self.current_field == field_id:
            print(f"  [{self.term.reverse(self.input_buffer)}]")
        else:
            print(f"  [{self._display_value(self._get_field_value(field_id))}]")
        print()

    def _draw_state_selection(self):
        self._draw_input_field('Number of states', self.STATE_COUNT_FIELD)

    def _draw_prob_selection(self):
        for i in range(self.state_count - 1):
            self._draw_input_field(f"Move probability {i + 1}:", i + self.PROB_FIELDS_OFFSET)

    def _draw_pomdp_selection(self):
        self._draw_state_selection()
        self._draw_prob_selection()

    def _draw_header(self):
        print(self.term.clear())
        print(self.term.center(self.term.bold("Set the game options")))
        print()

        self.spec.select_write_game_msg(self)

        print(self.term.center("Type numbers, Tab: next field, Enter: submit, Q: quit"))
        print()

    def _draw_additional_info_selection(self):
        for i, val in enumerate(self.additional_fields):
            self._draw_input_field(val[0], i)

    def _get_field_count(self):
        return self.STATE_COUNT_FIELD + self.state_count

    def _move_field(self, move_count: int):
        self.current_field = (self.current_field + move_count) % self._get_field_count()
        self._load_field_input()

    def _handle_tabs(self, move: int):
        self._save_current_input()
        self._move_field(move)

    def draw(self):
        self._draw_header()
        self._draw_additional_info_selection()
        self._draw_pomdp_selection()
        self._draw_error()

        key = self.term.inkey(timeout=0.1)

        if key is None:
            return self._get_incomplete()

        if key.lower() == "q":
            return self._get_exit()

        if key.name == "KEY_ENTER" or key.code == 10:
            self._handle_enter()
            return self._get_complete()

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

        if key.isascii():
            self.input_buffer += key
            return self._get_incomplete()

        return self._get_incomplete()

    def _handle_enter(self):
        self._save_current_input()

    def _get_converter(self):
        if self.current_field < self.STATE_COUNT_FIELD:
            return self.additional_fields[self.current_field][2]
        if self.current_field == self.STATE_COUNT_FIELD:
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

    def _set_state_count(self, value: int):
        if self.MIN_STATE_COUNT <= value <= self.MAX_STATE_COUNT:
            self.state_count = value
            self._update_probs(value - 1)

    def _set_prob_val(self, p_idx: int, value: float):
        if p_idx < self.state_count - 1 and self.PROB_LOWER_BOUND < value < self.PROB_UPPER_BOUND:
            self.probabilities[p_idx] = value

    def _set_value(self, value):
        if self.current_field < self.STATE_COUNT_FIELD:
            self.additional_fields_vals[self.current_field] = value
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
            self.input_buffer = ""
            self._set_error_msg("Invalid value for this field")
            return

        self._clear_error_msg()
        self._set_value(value)

        self.input_buffer = ""

    def _load_field_input(self):
        if self.input_buffer != '':
            return
        if self.current_field < self.STATE_COUNT_FIELD:
            self.input_buffer = self._display_value(self.additional_fields_vals[self.current_field])
        elif self.current_field == self.STATE_COUNT_FIELD:
            self.input_buffer = self._display_value(self.state_count)
        elif self.current_field < self._get_field_count():
            self.input_buffer = self._display_value(self.probabilities[self.current_field - self.PROB_FIELDS_OFFSET])

    def _get_result(self):
        self.spec.set_more_info(self.additional_fields_vals)
        return GameConfig([prob(p) for p in self.probabilities], self.spec)
