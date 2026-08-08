from blessed import Terminal
from enum import Enum


class POMDPGameState(Enum):
    RUNNING = 0
    WON = 1
    LOST = 2


class POMDPGameScreen:
    CONTAINER_WIDTH = 35
    CONTAINER_CHAR = "█"
    EMPTY_CHAR = " "
    WIN_THRESHOLD = 0.95
    
    def __init__(self, term: Terminal):
        self.move_count = 0
        self.probabilities = []
        self.states = 0
        self.term = term
        self.config = None
        self.belief = []
        self.current_state = 0
        self.game_state = POMDPGameState.RUNNING
        self.move_history = []
    
    def set_config(self, move_count: int, probs: list[float]):
        """Set the game configuration from the selection screen."""
        self.move_count = move_count
        self.probabilities = probs
        self.states = len(self.probabilities) + 1
        self.reset_game()
    
    def reset_game(self):
        self.belief = [0.0 for _ in range(self.states)]
        self.belief[0] = 1.0
        self.current_state = 0
        self.game_state = POMDPGameState.RUNNING
        self.move_history = []
        self.input_buffer = ""
        self.input_cursor_pos = 0
        self.showing_input = False
    
    def _apply_action_by_index(self, action_idx: int):
        if self.game_state != POMDPGameState.RUNNING:
            return
        if action_idx < 0 or action_idx >= len(self.probabilities):
            return
        
        p = self.probabilities[action_idx]
        new_belief = [0.0] * len(self.belief)
        
        for state in range(len(self.belief)):
            if state == action_idx and self.belief[state] > 0:
                new_belief[state + 1] += self.belief[state] * p
                new_belief[state] += self.belief[state] * (1 - p)
            elif state > 0:
                new_belief[state] += self.belief[state]
            else:
                new_belief[state] += self.belief[state]
        
        self.belief = new_belief
        self.move_history.append(action_idx)
        self.current_state = action_idx + 1
        
        if self.belief[-1] >= self.WIN_THRESHOLD:
            self.game_state = POMDPGameState.WON
        if self.current_state >= len(self.belief):
            if self.belief[-1] < self.WIN_THRESHOLD:
                self.game_state = POMDPGameState.LOST
    
    def _apply_action_from_input(self):
        if not self.input_buffer:
            return False
        try:
            action_idx = int(self.input_buffer)
            self._apply_action_by_index(action_idx)
            self.input_buffer = ""
            self.input_cursor_pos = 0
            self.showing_input = False
            return True
        except ValueError:
            return False

    
    def _draw_container(self, probability: float, is_final: bool = False, is_current: bool = False):
        fill_count = int(probability * self.CONTAINER_WIDTH)
        fill_str = self.CONTAINER_CHAR * fill_count
        empty_str = self.EMPTY_CHAR * (self.CONTAINER_WIDTH - fill_count)
        
        if is_final:
            if self.game_state == POMDPGameState.WON:
                content = self.term.green(fill_str + empty_str)
            else:
                content = self.term.yellow(fill_str + empty_str)
        elif is_current:
            content = self.term.cyan(fill_str + empty_str)
        else:
            content = self.term.blue(fill_str + empty_str)
        
        return f"[{content}]"
    
    def draw(self):

        print(self.term.clear())
        
        print(self.term.center(self.term.bold("POMDP LINE GAME")))
        print()
        
        print(self.term.center("States (containers with belief mass):"))
        print()
        
        containers = []
        labels = []
        for i in range(len(self.belief)):
            is_final = (i == len(self.belief) - 1)
            is_current = (i == self.current_state)
            containers.append(self._draw_container(self.belief[i], is_final, is_current))
            labels.append(f"[{i}]")
        
        print(self.term.center(" ".join(containers)))
        print(self.term.center(" ".join(labels)))
        print()
        
        print(self.term.center("Belief:"))
        belief_strs = []
        for i, prob in enumerate(self.belief):
            prob_percent = f"{prob*100:.1f}%"
            if i == len(self.belief) - 1:
                prob_percent = self.term.yellow(prob_percent)
            belief_strs.append(f"S{i}={prob_percent}")
        print(self.term.center("  ".join(belief_strs)))
        print()
        
        # Game status
        if self.game_state == POMDPGameState.WON:
            print(self.term.center(self.term.green(self.term.bold("YOU WON! Final state reached!"))))
        elif self.game_state == POMDPGameState.LOST:
            print(self.term.center(self.term.red(self.term.bold("GAME OVER"))))
        else:
            print(self.term.center(f"State: {self.current_state}, Actions: 0-{len(self.probabilities)-1}"))
        
        print()
        
        # Input field
        if self.showing_input:
            prefix = "Action: [" + self.input_buffer[:self.input_cursor_pos]
            suffix = self.input_buffer[self.input_cursor_pos:]
            input_str = prefix + self.term.reverse(suffix) + "]"
            print(self.term.center(input_str))
        else:
            print(self.term.center("Action: [Press I to input]"))
        
        print()
        print(self.term.center("I: Input action, A: Auto all, R: Reset, B: Back, Q: Quit"))
        
        # Handle input
        key = self.term.inkey(timeout=0.1)
        
        if self.showing_input:
            if key.name == "KEY_ENTER" or key.code == 10:
                if self._apply_action_from_input():
                    pass
                self.showing_input = False
            elif key.name == "KEY_ESCAPE":
                self.showing_input = False
                self.input_buffer = ""
                self.input_cursor_pos = 0
            elif key.name == "KEY_BACKSPACE" or key.code == 127:
                if self.input_cursor_pos > 0:
                    self.input_buffer = self.input_buffer[:self.input_cursor_pos - 1] + self.input_buffer[self.input_cursor_pos:]
                    self.input_cursor_pos -= 1
            elif key.name == "KEY_DC":
                if self.input_cursor_pos < len(self.input_buffer):
                    self.input_buffer = self.input_buffer[:self.input_cursor_pos] + self.input_buffer[self.input_cursor_pos + 1:]
            elif key.name == "KEY_LEFT":
                self.input_cursor_pos = max(0, self.input_cursor_pos - 1)
            elif key.name == "KEY_RIGHT":
                self.input_cursor_pos = min(len(self.input_buffer), self.input_cursor_pos + 1)
            elif key.isdigit():
                self.input_buffer = self.input_buffer[:self.input_cursor_pos] + key + self.input_buffer[self.input_cursor_pos:]
                self.input_cursor_pos += 1
            return None
        else:
            if key.lower() == "i":
                self.showing_input = True
                self.input_buffer = ""
                self.input_cursor_pos = 0
            elif key.lower() == "r":
                self.reset_game()
            elif key.lower() == "a":
                self.auto_play_all()
            elif key.lower() == "b":
                return "back"
            elif key.lower() == "q":
                return "quit"
            elif key.isdigit():
                action_idx = int(key)
                self._apply_action_by_index(action_idx)
            
        return None
