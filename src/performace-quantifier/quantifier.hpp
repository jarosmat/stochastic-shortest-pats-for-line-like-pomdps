#ifndef INC_2026_JAROS_QUANTIFIER_HPP
#define INC_2026_JAROS_QUANTIFIER_HPP

#include "prob.hpp"
#include "belief.hpp"
#include "lineLikePomdp.hpp"

//this function is operating under the assumption, that sum of elements of probs is 1
std::size_t sampleIndex(const std::vector<prob> &probs);

template<typename IT>
bool simulateThreshold(const belief &startb, const lineLikePomdp &pomdp, std::size_t T, IT &policy) {
    auto state = sampleIndex(startb.bel);
    if (pomdp.IsFinalState(state)) {
        return true;
    }
    for (std::size_t i = 0; i < T; ++i) {
        const std::size_t action = *policy;
        ++policy;
        auto nextb = pomdp.SimulateAction(action, state);

        state = sampleIndex(nextb.bel);

        if (pomdp.IsFinalState(state)) {
            return true;
        }
    }
    return false;
}

template<typename IT>
prob quantifyThreshold_s(const belief &startb, const lineLikePomdp &pomdp, std::size_t T, IT &policy,
                         const std::size_t reps = 10000) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < reps; ++i) {
        if (simulateThreshold(startb, pomdp, T, policy)) {
            ++count;
        }
    }

    return prob(static_cast<double>(count) / static_cast<double>(reps));
}

#endif //INC_2026_JAROS_QUANTIFIER_HPP
