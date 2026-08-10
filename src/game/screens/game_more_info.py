def max_prob_t_tries_more_info(screen):
    mvs = screen.extra_vals.get("max_moves")
    if mvs is not None:
        print(screen.term.center(f"Actions available: [{mvs - len(screen.actions)}/{mvs}]"))
