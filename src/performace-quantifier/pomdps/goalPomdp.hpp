#ifndef INC_2026_JAROS_GOALPOMDP_HPP
#define INC_2026_JAROS_GOALPOMDP_HPP
#include <cstddef>
#include <vector>
#include "prob.hpp"
#include "belief.hpp"

class costFunction {
    const std::size_t action_c;
    const std::size_t state_c;

    // represents a matrix with row i representing the costs of action in states
    std::vector<double> costs;

public:
    costFunction(std::vector<double> &&costs, std::size_t actions, std::size_t states);

    double operator ()(std::size_t action, std::size_t state) const;
};


class goalPomdp {
};


#endif //INC_2026_JAROS_GOALPOMDP_HPP