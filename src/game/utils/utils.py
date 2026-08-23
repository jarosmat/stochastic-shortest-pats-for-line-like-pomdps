from pypomdp import prob

DISPLAYED_PRECISION = 10


def display_percentage(value: prob):
    return f"{float(value) * 100:.{DISPLAYED_PRECISION}f}%"
