from abc import ABC, abstractmethod
from typing import TypeVar, List
from collections.abc import Callable
from multiprocessing import Event, Queue

T = TypeVar('T')


class BaseGameSpec(ABC):
    def __init__(self):
        self.best_result = None
        self.result_queue = Queue()

    @abstractmethod
    def intro_game_description(self) -> str:
        pass

    @abstractmethod
    def select_additional_fields_specs(self) -> List[tuple[str, T, Callable[[str], T]]]:
        pass

    @abstractmethod
    def select_write_game_msg(self, screen):
        pass

    @abstractmethod
    def game_validate(self, screen) -> bool:
        pass

    @abstractmethod
    def game_write_more_info(self, screen):
        pass

    @abstractmethod
    def game_write_completed_info(self, screen):
        pass

    @abstractmethod
    def set_more_info(self, info_vals: list):
        pass

    @abstractmethod
    def calculate_best_res(self, stop_event: Event, move_probs: list[float]):
        pass

    def _get_best_moves(self):
        if self.best_result is not None:
            return self.best_result
        try:
            res = self.result_queue.get_nowait()
            self.best_result = res
            return self.best_result
        except Exception:
            return None
