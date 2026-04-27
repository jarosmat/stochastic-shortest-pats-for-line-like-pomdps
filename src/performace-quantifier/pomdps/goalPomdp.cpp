#include "goalPomdp.hpp"

costFunction::costFunction(std::vector<double> &&costs, const std::size_t actions,
                           const std::size_t states) : action_c(actions), state_c(states), costs(std::move(costs)) {
}

double costFunction::operator()(const std::size_t action, const std::size_t state) const {
    return costs.at(action * state_c + state);
}

transitionProb::transitionProb(const std::size_t states, std::vector<prob> &&probs) : state_c(states),
    probs(std::move(probs)) {
}

prob transitionProb::operator()(const std::size_t target, const std::size_t start) const {
    return probs.at(start * state_c + target);
}

observationProb::observationProb(const std::size_t observations, const std::size_t states,
                                 std::vector<prob> &&probs) : observation_c(observations), state_c(states),
                                                              probs(std::move(probs)) {
}

prob observationProb::operator()(const std::size_t observation, const std::size_t state) const {
    return probs.at(state * observation_c + observation);
}
