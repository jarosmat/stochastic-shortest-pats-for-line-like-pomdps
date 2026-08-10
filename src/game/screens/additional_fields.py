DEF_MOVE_COUNT = 10
MAX_MOVE_COUNT = 100


def max_t_tries_num_moves_conv(val: str):
    conv = int(val)
    if 0 < conv < MAX_MOVE_COUNT:
        return conv
    raise ValueError


MAX_PROB_T_TRIES_NUM_MOVES_SPEC = ("Number of moves", DEF_MOVE_COUNT, max_t_tries_num_moves_conv)
