from typing import Literal

from pypomdp import LineLikePomdp, belief

class GameResult:
    def __init__(self, game_type: Literal[int], pomdp: LineLikePomdp, bel: belief, actions: list[int]):
        self.game_type = game_type
        self.pomdp = pomdp
        self.bel = bel
        self.actions = actions

