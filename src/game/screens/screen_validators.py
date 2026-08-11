def validate_max_prob_t_tries_win(screen):
    mvs = screen.extra_vals.get("max_moves")
    if mvs is not None:
        return len(screen.actions) == mvs

    return False

def validate_min_threshold_at_end(screen):
    mvs = screen.extra_vals.get("threshold")

    if mvs is not None:
        return screen.pomdp.is_at_end_prob(screen.belief, mvs)

    return False
