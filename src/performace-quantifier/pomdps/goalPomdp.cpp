#include "goalPomdp.hpp"

costFunction::costFunction(std::vector<double> &&costs, const std::size_t actions,
                           const std::size_t states) : action_c(actions), state_c(states), costs(std::move(costs)) {
}

double costFunction::operator()(const std::size_t action, const std::size_t state) const {
    return costs.at(action * state_c + state);
}
