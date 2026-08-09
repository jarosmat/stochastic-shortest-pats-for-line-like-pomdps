def max_prob_t_tries_more_info(screen):
    print(screen.term.center(f"Actions available: [{screen.max_moves - len(screen.actions)}/{screen.max_moves}]"))
