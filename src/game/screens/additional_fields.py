DEF_MOVE_COUNT = 10
MAX_MOVE_COUNT = 100


def max_t_tries_num_moves_conv(val: str):
    conv = int(val)
    if 0 < conv < MAX_MOVE_COUNT:
        return conv
    raise ValueError


MAX_PROB_T_TRIES_NUM_MOVES_SPEC = ("Number of moves", DEF_MOVE_COUNT, max_t_tries_num_moves_conv)

DEF_THRESHOLD = 0.5
MAX_THRESHOLD = 0.8


def move_threshold_conv(val: str):
    conv = float(val)
    if 0 < conv < MAX_THRESHOLD:
        return conv
    raise ValueError


MOVE_THRESHOLD_SPEC = ("Amount of matter to move", DEF_THRESHOLD, move_threshold_conv)
