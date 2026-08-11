from .utils import display_percentage

def max_prob_t_tries_complete_drawer(screen):
    print(screen.term.center("Game Complete!"))
    print()
    res = screen.pomdp.prob_at_end(screen.belief)
    print(screen.term.center(f"You managed to move {display_percentage(res)} of matter to end state"))
    print()
    print(screen.term.center("It is possible to move {some amount}"))
    print()

def min_threshold_complete_drawer(screen):
    print(screen.term.center("Game Complete!"))
    print()
    mvs = len(screen.actions)
    print(screen.term.center(f"You managed to move the desired amount of matter to end in {mvs} moves"))
    print()
    print(screen.term.center("It is possible to do so in {some amount}"))
    print()

