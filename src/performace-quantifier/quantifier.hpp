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
        const action action = *policy;
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
        policy.reset();
        if (simulateThreshold(startb, pomdp, T, policy)) {
            ++count;
        }
    }

    return prob(static_cast<double>(count) / static_cast<double>(reps));
}

template<typename IT>
std::vector<prob> quantifyThreshold_m(belief &&startb, const lineLikePomdp &pomdp, std::size_t T, IT &policy) {
    if (T == 0) {
        return pomdp.GetFinalStatesProbs(std::move(startb));
    }
    action action = *policy;
    auto interRes = pomdp.UpdateBelief(std::move(startb), action);
    ++policy;

    for (std::size_t i = 1; i < T; ++i) {
        action = *policy;
        interRes = pomdp.UpdateBelief(std::move(interRes), action);
        ++policy;
    }

    return pomdp.GetFinalStatesProbs(std::move(interRes));
}

template<typename IT>
std::size_t simulateExpectation(const belief &startb, const lineLikePomdp &pomdp, IT &policy) {
    std::size_t count = 0;
    auto state = sampleIndex(startb.bel);

    while (!pomdp.IsFinalState(state)) {
        const action action = *policy;
        ++policy;
        auto nextb = pomdp.SimulateAction(action, state);
        ++count;

        state = sampleIndex(nextb.bel);
    }

    return count;
}

template<typename IT>
double quantifyExpectation_s(const belief &startb, const lineLikePomdp &pomdp, IT &policy, std::size_t reps = 10000) {
    policy.reset();
    double avg = simulateExpectation(startb, pomdp, policy);

    for (std::size_t i = 2; i <= reps; ++i) {
        policy.reset();
        const double newVal = simulateExpectation(startb, pomdp, policy);
        avg = avg + (newVal - avg) / static_cast<double>(i);
    }

    return avg;
}
#endif //INC_2026_JAROS_QUANTIFIER_HPP
