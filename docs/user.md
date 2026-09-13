# User documentation

## Installation

### Dependencies

The following packages must be installed:

- `cmake`
- `make`
- C++ compiler
- `python3`
- `python3-dev`
- `python3Packages.matplotlib`
- `python3Packages.numpy`
- `python3Packages.pybind11`
- `python3Packages.blessed`

## Building the Project

```shell
source build.sh
```

# POMDP plotter

# POMDP game

## Overview

POMDP game is a terminal user interface game that visualizes Partially Observable Markov Decision
Processes (POMDPs). Player moves matter between different containers, this represents the shifts in belief
of current state.

There are two game modes:

- **Max Prob T Tries**: Maximize the amount of matter in the final container after a fixed number of moves
- **Min Moves Before Threshold**: Move a specified percentage of matter to the final container in as few moves as
  possible

First the player selects game mode, then configuration of the game mode and lastly gets to play the game.
After completing the objective of the game the player is revealed the optimal strategy.

## Running the Game

```shell
./run_game.sh
```

## Examples

**After running the game - game mode selection:**

![Intro screen](img/intro-screen.png)

**Configuration of the game mode:**

![Game config screen](img/game-config-screen.png)

**Playing the game:**

![Game screen](img/game-screen-game-start.png)

**Game completed:**

![Game completed](img/game-screen-game-completed.png)

## Game Flow

1. **Intro Screen**: Select which game mode to play
2. **Number Selection Screen**: Configure game parameters
3. **POMDP Game Screen**: Play the selected game

## Controls

### All Screens

| Key             | Action                |
|-----------------|-----------------------|
| `Q`             | Quit the game         |
| `Arrow Up/Down` | Navigate menu options |
| `Enter`         | Confirm selection     |

### Number Selection Screen

| Key                    | Action                      |
|------------------------|-----------------------------|
| `Tab`                  | Move to next field          |
| `Shift+Tab`            | Move to previous field      |
| `Backspace` / `Delete` | Delete character from input |
| `0-9`                  | Enter numeric value         |
| `Enter`                | Submit configuration        |

### POMDP Game Screen

| Key         | Action                      |
|-------------|-----------------------------|
| `0-9`       | Enter action number         |
| `Backspace` | Delete character from input |
| `Enter`     | Execute entered action      |
| `R`         | Reset the current game      |
| `Q`         | Quit to exit                |

## Game Modes

### Mode 1: Max Matter in Final Container After T Moves

**Objective:** Maximize the probability mass in the final (rightmost) state after exactly T moves.

**Configuration:**

- **Number of moves (T)**: How many actions you can take (default: 10, max: 100)
- **Number of states**: Number of containers/positions (default: 3, range: 3-5)
- **Move probabilities**: For each gap between states, the probability of successfully moving right (default: 0.5,
  range: 0.1-1.0)

**Win Condition:** Complete all T moves. The game shows how much matter you moved to the end and
show the optimal solution.

**Gameplay:**

- Enter action numbers (0 to states-2) to attempt moving from that state
- After T moves, the game completes and shows your result vs optimal

### Mode 2: Min Number of Moves Before Threshold

**Objective:** Move a specified percentage of matter to the final state in as few moves as possible.

**Configuration:**

- **Amount of matter to move**: Target threshold percentage (default: 0.5, max: 0.8)
- **Number of states**: Number of containers/positions (default: 3, range: 3-5)
- **Move probabilities**: For each gap between states, the probability of successfully moving right (default: 0.5,
  range: 0.1-1.0)

**Win Condition:** Reach or exceed the threshold probability in the final state.

**Gameplay:**

- Enter action numbers to move belief mass rightward
- The game completes when the final state's probability >= threshold
- Results show your move count vs the optimal minimum

## Game Mechanics

### States and Actions

- States are numbered `0` to `N-1` where N is the number of states
- From state `i`, you can attempt to move right to state `i+1`
- Action `i` means "attempt to move from state i to i+1"
- Each move has a configurable probability of success (move probability)
- Valid actions are `0` through `N-2` (you cannot move from the final state)

### Belief

- The belief represents the probability distribution over states
- Initially, all belief mass is in state 0: `S0=100%, S1=0%, S2=0%, ...`
- Each action updates the belief based on the move probabilities
- The visualization shows this distribution as filled containers

### Optimal Solution

After completing a game, the system computes the optimal solution in the background and displays:

- The optimal policy (sequence of moves)
- The optimal result (maximum achievable probability or minimum moves)
