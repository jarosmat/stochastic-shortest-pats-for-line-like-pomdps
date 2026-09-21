## Project description

Project is focused on finding the best policies for certain type of POMDPs (Partially Observable Markov Decision
Processes). POMDP is a mathematical framework for modeling decisions of agent under uncertainty. Agent can make
decisions based on its belief of its current state - it does not know its exact positions, make observations about its
positions, its goal is to get to final state. Formally it is a given by tuple:
$ (S, b_{0}, T \subseteq S, A, \{P_{a} (s'|s) |a \in A, s',s \in S \}, \{c (a, s)| a \in A, s \in S /T\}, O, \{Q_a (o|s)| a \in A, o \in O, s \in S\}) $

1. $S$ is non-empty state space
2. $b_0$ is initial belief of agents position - distribution over $S$
3. $T\subseteq S$ is set of final states
4. $A$ is the set of possible actions
5. $P_a (s'|s)$ is the probability of moving to state $s'$ from state $s$ after taking action $a$
6. $c (a, s)$ is the cost of taking an action $a$ in state $s$
7. $O$ is the set of possible observations
8. $Q_a (o|s)$ is the probability of observing $o \in O$ after taking action $a \in a$ and transitioning to
   state $s \in S$

The type in question has a state space of $n \in N, n\geq 3$ states, forming a line. This means that there is only one
final state, the state $n$, the initial belief is that the agent is in the state $0$ with probability $1$, there
are $[n-1]$ possible actions, probabilities $ 0 < P_i (i + 1| i) < 1$ are arbitrary, with any other $P_a (s'|s)$ being
zero, cost of taking any action in any state is $1$, there are two observations - agent is in the final state and agent
is not in the final state, lastly agent observes that it is not in final state with probability of $1$ after moving into
non-final state and with probability $1$ it observes it is in final state after moving there.

The project is focusing on instance, where $n=3$ and trying to find optimal strategies with respect to these two
quantities:

1. Maximize probability of being in final state after fixed number of moves
2. Considering a threshold probability $t$, two quantities:
    1. minimizing number of moves before having probability of at least $t$ of being in the final state
    2. minimizing expected number of moves before reaching the last state with probability of at least $t$

### `src/strategy-plotter/contour_max_prob.cpp`

Visualizes best strategies for maximizing probability of being in final state after $10$ steps for move probabilities
ranging from $0.01$ to $0.99$ with $0.01$ granularity. The number it shows is the number of actions $0$ to take before
using the remaining available actions to take action $1$.

### `src/strategy-plotter/contour_percentile.cpp`

Considering threshold probability $t$, visualizes the best strategies for minimizing number of moves before having at
least probability $t$ of being in final state and minimizing expected number of moves before reaching the final state
with probability of at least $t$. The quantity optimized can be specified via options:

- `--prob1-count` - number of different probabilities of moving from state 0 to state 1
- `--prob2-count`  - number of different probabilities of moving from state 1 to state 2
- `--grain` - the difference between two probabilities that are right next to each other
    - if start with 0.01 (`--start-probX`) and set the grain to 0.01 with three probabilities (`--probX-count`), it will
      be plotted for these probabilities: 0.01, 0.02, 0.03
- `--start-prob1` - lowest probability of moving from state 0 to state 1
- `--start-prob2` - lowest probability of moving from state 1 to state 2
- `--colors` - number of different colors for matplotlib ot use. Not important parameter, by default set to 1.
- `--threshold-val` - value of threshold, probability of being in final state we want to achieve.
- `--display-val` - what information about the strategies to display, two possible values
    - `string-val` - actual moves, displays `A` for move from state 0 to state 1, `B` for move from state 1 to state 2
    - `string-len` - displays the length of optimal strategy that gets us to have probability at least `threshold` of
      being in final state
- `--quantity` - what to optimize about the strategies, two possible values:
    - `min-exp` - minimizes expected number of moves before having probability of at least `threshold` of being in the
      final state.
    - `min-str-len` - minimizes the length of strategy that gets us probability of at least `threshold` of being in the
      final state.
- `--trim-b` - flag, that is there or not. If present, then after the optimal strategies are found, trailing `B` actions
  are removed and strategies are displayed without them

### `src/game`

This folder contains simple TUI (Terminal User Interface) game, that lets user play as an agent, while visualizing their
moves. User can try to maximize probability of being in final state after fixed amount of moves or minimize number of
moves before having probability of at least some threshold $t$ of being in final state. After user completes the game
(uses all of their moves or moves enough probability to final state) an optimal solution is revealed. For user interface
and more information, refer to [User documentation](docs/user.md).

## Documentation

- [Programmer documentation](docs/programmer.md)
- [User documentation](docs/user.md)
- [Interesting examples](docs/examples.md)

## Build

For building, the following packages must be installed:

- `cmake`
- `make`
- C++ compiler
- `python3`
- `python3-dev`
- `python3Packages.matplotlib`
- `python3Packages.numpy`
- `python3Packages.pybind11`
- `python3Packages.blessed`

To build the project, run

```shell 
source build.sh
```

To run the game run:

```shell
./run_game.sh
```