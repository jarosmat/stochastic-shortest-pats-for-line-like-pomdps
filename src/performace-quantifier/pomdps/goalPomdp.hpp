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

class transitionProb {
    std::size_t state_c;
    std::vector<prob> probs;

public:
    transitionProb(std::size_t states, std::vector<prob> &&probs);

    // represents a matrix with row i, represents probabilities to end up in states from state i, after taking action this
    // function is associated with
    prob operator ()(std::size_t target, std::size_t start) const;
};

class observationProb {
    std::size_t observation_c;
    std::size_t state_c;

    // represents a matrix with row i, represents probabilities of observing all observations upon entering state i
    // when indexed, first index is state we entered and second is observation
    std::vector<prob> probs;

public:
    observationProb(std::size_t observations, std::size_t states, std::vector<prob> &&probs);

    prob operator ()(std::size_t observation, std::size_t state) const;
};


template<typename CF, typename TF, typename OF>
class goalPomdp {
};


#endif //INC_2026_JAROS_GOALPOMDP_HPP