# Programmer Documentation

## Application design

Core components for POMDP computations are written in C++. The POMDP game is written in Python 3, using the blessed TUI
library. Python library pybind11 is used to create bindings for the POMDP components written in C++ to Python.

The game is organized into three screens. One used to select game mode, second to select configuration and third to play
the game. The game utilizes multiprocessing to compute the optimal strategy - a separate process is spawned, that runs
the C++ solver.

### `performace-quantifier/`

This folder contains most of the components for modeling POMDPs and for quantifying the performance of strategies.
It is structured into three folders and files:

- `belief`
- `pomdps`
- `probability`
- `quantifier.cpp` + `quantifier.hpp`

#### `probability`

This folder contains a type used for modeling probability.

- `struct prob`
    - represents probability, uses `double` to represent it internally
    - exposes number of common operators on probability values
    - `prob operator+(const prob &other) const`
    - `prob &operator+=(const prob &other)`
    - `prob &operator-=(prob other)`
    - `prob &operator*=(prob other)`
    - `prob &operator/=(prob other)`
    - `prob operator-(const prob &other) const`
    - `prob operator*(const prob &other) const`
    - `prob operator/(const prob &other) const`
    - `bool operator==(const prob &other) const`
    - `bool operator!=(prob other) const`
    - `bool operator<(const prob &other) const`
    - `bool operator>(const prob &other) const`
    - `bool operator<=(const prob &other) const`
    - `bool operator>=(const prob &other) const`
    - `explicit prob(double val = 0.0)` - explicit constructor, constructs probability value with specified `double`
      value
    - `friend prob min(const prob &a, const prob &b)` - computes the minimum out of the two probability values
    - `friend prob max(const prob &a, const prob &b)` - computes the maximum out of the two probability values
    - `explicit operator double() const` - explicit operator used for conversion to `double`
    - `friend std::ostream &operator<<(std::ostream &os, const prob &val)` - friend operator for dumping probability
      values to output streams
    - `friend double operator*(std::size_t other, prob p)`
    - `friend double operator/(double other, prob p)`

#### `belief`

This folder contains type used for modeling belief over POMDP states.

- `struct belief`
    - represents the belief over POMDP states using vector of probabilities, where `i`-th value represents the
      probability of being in state `i`
    - `std::vector<prob> bel` - represents the vector of probabilities
    - `prob &operator[](std::size_t i)` - operator for accessing the desired probability
    - `const prob &operator[](std::size_t i) const`
    - `explicit belief (std::size_t states)` - initializes belief with `states` states
    - `explicit belief (std::vector<prob> &&val)` - initializes belief with `val` as an internal value
    - `explicit belief (const std::vector<prob> &val)` - initializes belief with `val` as an internal value

#### `pomdps`

This folder contains classes for representation of POMDPs

- `class line_like_pomdp`
    - represents a POMDP with states in a line where from `i`-th state one can move only to `i+1`-th state using `i`-th
      action
    - only observations are whether we are in the final state or not
    - the final state is the last state in the line
    - `line_like_pomdp(std::vector<prob> &&probs)` - constructor, initializes POMDP with move probabilities between
    - `line_like_pomdp(const std::vector<prob> &probs)` - constructor, initializes POMDP with move probabilities between
    - `belief update_belief(belief &&bel, action action) const` - updates belief by applying action
    - `belief update_belief(const belief &bel, action action) const` - updates belief by applying action
    - `belief simulate_action(action action, state state) const` - simulates applying an action from a specific state
    - `bool is_final_state(state state) const` - returns true if the state is the final state
    - `std::vector<prob> get_final_states_probs(belief &&bel) const` - returns probabilities of being in the final
      states for each final state
    - `double get_expected_cost(const belief &belief, action a) const` - returns expected cost of an action
    - `bool end_belief(const belief &bel) const` - returns true if probability of being in final state in belief `bel`
      is `1`
    - `std::size_t states() const` - returns number of states
    - `bool is_at_end_prob(const belief &bel, prob prob) const` - returns true if probability of being at end state is
      at least
      `prob`
    - `prob prob_at_end(const belief &bel) const` - returns probability of being at the end state

#### `quantifier.hpp` + `quantifier.cpp`

These files contain function templates for performance quantification.

- `std::size_t sample_index(const std::vector<prob> &probs)` - samples an index from a probability distribution

-

`template<typename It, typename P> bool simulate_threshold(const belief &startb, const P &pomdp, const std::size_t t, It &policy)` -
simulates a policy for `t` steps and returns whether final state was reached

-

`template<typename It, typename P> prob quantify_threshold_s(const belief &startb, const P &pomdp, const std::size_t t, It &policy, const std::size_t reps)` -
quantifies the probability of reaching the final state after `t` steps by running `reps` simulations

-

`template<typename It, typename P> prob quantify_threshold_s(const belief &startb, const P &pomdp, const std::size_t t, It &policy)` -
quantifies the probability of reaching the final state with default 10000 repetitions

-

`template<typename It, typename P> std::vector<prob> quantify_threshold_m(belief &&startb, const P &pomdp, const std::size_t t, It &policy)` -
returns probabilities of being in final states after `t` steps

-

`template<typename It, typename P> std::size_t simulate_expectation(const belief &startb, const P &pomdp, It &policy)` -
simulates a policy and returns the number of steps taken to reach a final state

-

`template<typename It, typename P> double quantify_expectation_s(const belief &startb, const P &pomdp, It &policy, const std::size_t reps)` -
quantifies the expected number of steps before reaching final state by running `reps` simulations

- `template<typename It, typename P> double quantify_expectation_s(const belief &startb, const P &pomdp, It &policy)` -
  quantifies the expected number of steps with default 10000 repetitions

-

`template<typename It, typename P> std::pair<std::size_t, double> quantify_steps_until_threshold_m(belief &&startb, const P &pomdp, It &policy, prob threshold)` -
returns a pair of steps taken and expected steps until threshold probability is reached

### `game-solvers/`

This folder contains function that compute the best strategies for POMDP game types.

#### `max_prob_t_tries_solver.hpp`

- `class max_prob_t_tries_solver`
  -
  `static std::pair<expanded_policy, prob> solve(const line_like_pomdp &pomdp, std::size_t t, const belief &start_b)` -
  searches strategies to find the one maximizing the probability of being in final state after `t` steps. Uses
  `combinations_with_repetitions` to search all possible strategies. Uses combination with repetition because it is
  not optimal to play action `j < i` after playing strategy `i`

#### `min_moves_before_threshold_solver.hpp`

- `class min_moves_before_threshold_solver`
  -
  `static std::pair<periodic_policy, std::pair<std::size_t, double>> solve(const line_like_pomdp &pomdp, prob threshold, const belief &start_b)`  -
  finds the shortest strategy that guarantees `threshold` probability of being in the final state. Uses BFS, but only
  keeps information on how long the strategy was, then searches all `permutations_with_repetition` to find the one that
  actually guarantees that.

#### `combinations_with_repetition.hpp`

- `namespace detail::combination_iterator<V>` - iterator class for combinations with repetition
    - `using value_type = std::vector<val_t>`
    - `const value_type &operator*() const` - returns current combination
    - `combination_iterator &operator++()` - advances to next combination
    - `void operator++(int)` - advances to next combination
    - `friend bool operator==(const combination_iterator &it, const std::default_sentinel_t &)` - equality comparison
- `class combinations_with_repetition_view<R>` - view class for combinations with repetition
    - `iterator begin()` - returns iterator to first combination
    - `sentinel end() const` - returns sentinel for end
- `struct combinations_with_repetition` - range adaptor closure
    - `explicit combinations_with_repetition(const std::size_t r)` - constructor with repetition count
    - `template<std::ranges::viewable_range R> constexpr auto operator()(R &&rng) const` - creates a combinations view

#### `permutations_with_repetition.hpp`

- `namespace detail::permutation_iterator<V>` - iterator class for permutations with repetition
    - `using value_type = std::vector<val_t>`
    - `const value_type &operator*() const` - returns current permutation
    - `permutation_iterator &operator++()` - advances to next permutation
    - `permutation_iterator operator++(int)` - advances to next combination
    - `friend bool operator==(const permutation_iterator &it, const std::default_sentinel_t)` - equality comparison
- `class permutations_with_repetition_view<R>` - view class for permutations with repetition
    - `iterator begin() const` - returns iterator to first permutation
    - `sentinel end() const` - returns sentinel for end
- `struct permutations_with_repetition` - range adaptor closure
    - `template<std::ranges::viewable_range R> constexpr auto operator()(R &&rng) const` - creates a permutations view

### `policies/`

This folder contains the representations of policies.

#### `expanded-policy/`

This folder contains the representation of policy, that returns sorted integers from 0 to specified max value. Every
value
is returned at least once, it can be specified what values to play after one another by providing a sorted vector in
non-decreasing order. For every value in the vector, it will be played once more than the times it is in the vector.
For example: vector specified: `[0,1,2]` and maximum value: `3`, the policy will play `0,0,1,1,2,2,3`.
or vector: `[0,2]`, maximum value `3`, the policy will play: `0,0,1,2,2,3`.

- `class expanded_policy`
    - `expanded_policy(std::vector<std::size_t> &&vec, std::size_t max_v)` - constructor, initializes with actions and
      maximum value
    - `expanded_policy(const std::vector<std::size_t> &vec, std::size_t max_v)` - constructor, initializes with actions
      and maximum value
    - `value_type operator*() const` - returns current action
    - `expanded_policy &operator++()` - advances to next action
    - `void operator++(int)` - advances to next action
    - `void reset()` - resets to beginning

#### `periodic-policy/`

This folder contains representation of policy, that repeatedly plays specified sequence.

- `class periodic_policy`
    - `periodic_policy(std::vector<std::size_t> &&vec)` - constructor, initializes with action sequence
    - `periodic_policy(const std::vector<std::size_t> &vec)` - constructor, initializes with action sequence
    - `value_type operator*() const` - returns current action
    - `periodic_policy &operator++()` - advances to next action
    - `void operator++(int)` - advances to next action
    - `void reset()` - resets to beginning
    - `std::size_t len() const` - returns length of policy

### `strategy-plotter/`

This folder contains executables for visualizing the best strategies.

#### `plotting.hpp` + `plotting.cpp`

- `periodic_policy get_policy(const std::vector<std::size_t> &counts)` - creates a periodic policy from action counts
- `std::vector<prob> plot_probs(std::vector<prob> &&probs, std::size_t t)` - computes probabilities for plotting
- `std::size_t max(const std::vector<prob> &vals)` - returns index with maximum probability
-

`void plot_table(std::vector<prob> &&base_probs, std::vector<prob> &&table_probs, std::size_t t, std::ostream &stream)` -
outputs a table of strategy probabilities

#### `plotter.cpp`

Executable that accepts exactly two arguments, first file with pairs of probabilities and second a number of steps. and
computes the optimal count of first actions for each pair, when trying to have a strategy that has the biggest
probability of being in final state after specified number of moves.

#### `plot_table.cpp`

Executable that accepts exactly three arguments, first file with probabilities, second a number of steps and
third a dimension - number of states in POMDP minus. With the probabilities, dimension and count plots a table with
optimal strategies, using the `plot_table` function from `plotting.hpp`.

#### `contour_max_prob.cpp`

Executable that accepts no arguments. Creates contour plot, that visualizes optimal strategies for having the biggest
probability of being in final state after `t` steps. It displays only values for three state line like POMDPs, with move
probabilities ranging from 0.01 to 0.99, with 0.01 granularity. Values displayed are the counts of 0-th move to be
played (rest to the `t` value is 1-th move).

#### `contour_percentile.cpp`

Executable that accepts range of options. It creates a contour plot for the optimal strategies of having at least some
`threshold` probability of being in the final state. Uses BFS to explore policy state.

**Options:**

- `--prob1-count` - number of different probabilities of moving from state 0 to state 1
- `--prob2-count`  - number of different probabilities of moving from state 1 to state 2
- `--grain` - the difference between two probabilities that are right behind each other
    - if start with 0.01 (`--start-probX`) and set the grain to 0.01 with three probabilities (`--probX-count`), it will
      be plotted for
      these probabilities: 0.01, 0.02, 0.03
- `--start-prob1` - lowest probability of moving from state 0 to state 1
- `--start-prob2` - lowest probability of moving from state 1 to state 2
- `--colors` - number of different colors for matplotlib ot use. Not important parameter, by default set to 1.
- `--threshold-val` - value of threshold, probability of being in final state we want to achieve.
- `--display-val` - what information about the strategies to display, two possible values
    - `string-val` - actual moves, displayed with `A` for move from state 0 to state 1, `B` for move from state 1 to
      state 2
    - `string-len` - displays the length of optimal strategy that gets us to have probability at least `threshold` of
      being in final state
- `--quantity` - what to optimize about the strategies, two possible values:
    - `min-exp` - minimizes expected number of moves before having probability of at least `threshold` of being in the
      final state.
    - `min-str-len` - minimizes the length of strategy that gets us probability of at least `threshold` of being in the
      final state.
- `--trim-b` - flag, that is there or not. If present, then after the optimal strategies are found, trailing `B` actions
  are removed and strategies are displayed without them

### `python_bindings.cpp`

This file contains the Python bindings using the pybind11 library.

### `game/`

This folder contains the Python POMDP Line Game. It contains three folders and main file

- `main.py`
- `screens`
- `specs`
- `utils`

#### `main.py`

File to run when wanting to play the game. It initializes all screens and switches them when needed.

#### `screens/`

This folder contains all screen classes for the game.

- `class ScreenResult(Enum)` - enum for telling main what to do.
    - `EXIT = 0` - quit the application
    - `COMPLETE = 1` - screen complete, move to next screen
    - `INCOMPLETE = 2` - screen incomplete, stay on current screen

- `class ScreenBase(ABC)` - abstract base class for all screens
    - `term: Terminal` - blessed terminal instance
    - `input_buffer: str` - current user input
    - `err_msg: str` - error message to display
    - `__init__(self, term: Terminal)` - constructor
    - `draw(self) -> tuple[ScreenResult, dict | object]` - abstract method, draws the screen and handles inputs

- `class IntroScreen(ScreenBase)` - screen for selecting game type
    - `__init__(self, term: Terminal, specs: list[BaseGameSpec])` - constructor
    - `draw() -> tuple[ScreenResult, BaseGameSpec | dict]` - draws the screen with options for selecting game type

- `class GameConfig` - class containing game configuration
    - `__init__(self, move_probs: list[prob], spec: BaseGameSpec)` - constructor
    - `move_probs: list[prob]` - move probabilities for specified POMDP
    - `spec: BaseGameSpec` - game specification

- `class NumberSelectionScreen(ScreenBase)` - screen for selecting game configuration
    - `__init__(self, term: Terminal, spec: BaseGameSpec)` - constructor
    - `draw() -> tuple[ScreenResult, GameConfig | dict]` - draws the screen and saves the selected configration

- `class POMDPGameState(Enum)` - enum representing, whether game is completed or not
    - `RUNNING = 0` - game is in progress
    - `COMPLETE = 1` - game has completed

- `class POMDPGameScreen(ScreenBase)` - screen for playing the configured game
    - `__init__(self, term: Terminal)` - constructor
    - `set_config(conf: GameConfig)` - configures game with new settings
    - `reset_game()` - resets the game
    - `actions_taken_count() -> int` - returns number of actions taken
    - `draw() -> tuple[ScreenResult, None]` - draws the screen and handles game mechanics

#### `specs/`

This folder contains the classes for game type specifications.

- `class BaseGameSpec(ABC)` - abstract base class for all game specifications
    - `best_result` - optimal solution, when not computed, has value of `None`
    - `result_queue: Queue` - queue for getting results of computing optimal strategy in separate process
    - `intro_game_description() -> str` - return description to show in intro screen, when choosing game type
    - `select_additional_fields_specs() -> list[tuple[str, T, Callable[[str], T]]]` - returns specifications, on how to
      select game type specific configs
    - `select_write_game_msg() -> list[str]` - message to be shown, when specifying the game type config
    - `game_validate(pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> bool` - returns `True` if game is completed
    - `game_write_more_info(pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> list[str]` - returns information to
      display during the game
    - `game_write_completed_info(pomdp: LineLikePomdp, bel: belief, moves: list[int]) -> list[str]` - what to display
      after game is completed
    - `set_more_info(info_vals: list)` - sets values of game type specific configurations
    - `calculate_best_res(stop_event: Event, move_probs: list[float])` - runs in separate Process, computes the optimal
      solution

- `class MaxProbTTriesSpec(BaseGameSpec)` - game specification for maximizing the probability of being in the final
  state after `T` moves
    - `__init__(self)` - constructor
    - implements all abstract methods from `BaseGameSpec`

- `class MinMovesBeforeThresholdSpec(BaseGameSpec)` - game specification for minimizing the nuber of moves until certain
  probability of being in the final state has been reached
    - `__init__(self)` - constructor
    - implements all abstract methods from `BaseGameSpec`

#### `utils/`

This folder contains utility functions for the game.

- `DISPLAYED_PRECISION: int = 10` - number of decimal places for display
- `display_percentage(value: prob) -> str` - formats probability as percentage string with `DISPLAYED_PRECISION`
  decimals
