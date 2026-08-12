from abc import ABC, abstractmethod
from typing import TypeVar
from collections.abc import Callable

T = TypeVar('T')


class BaseGameSpec(ABC):
    @abstractmethod
    def intro_game_description(self) -> str:
        pass

    @abstractmethod
    def select_additional_fields_specs(self) -> list[tuple[str, T, Callable[[str], T]]]:
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
