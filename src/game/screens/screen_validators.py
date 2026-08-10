def validate_max_prob_t_tries_win(screen):
    mvs = screen.extra_vals.get("max_moves")
    if mvs is not None:
        return len(screen.actions) == mvs

    return False
