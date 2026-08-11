from .utils import display_percentage

def max_prob_t_tries_more_info(screen):
    mvs = screen.extra_vals.get("max_moves")
    if mvs is not None:
        print(screen.term.center(f"Actions available: [{mvs - len(screen.actions)}/{mvs}]"))

def min_moves_before_threshold_more_info(screen):
    thr = screen.extra_vals["threshold"]
    bel = screen.pomdp.prob_at_end(screen.belief)

    if thr is not None:
        print(screen.term.center(f"Already moved: {display_percentage(bel)} of matter, to end the game you need to move {thr} of matter"))
