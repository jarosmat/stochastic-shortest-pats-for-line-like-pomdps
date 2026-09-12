#ifndef INC_2026_JAROS_QUANTIFIER_HPP
#define INC_2026_JAROS_QUANTIFIER_HPP

#include "prob.hpp"
#include "belief.hpp"
#include "lineLikePomdp.hpp"

//this function is operating under the assumption, that sum of elements of probs is 1
std::size_t sampleIndex(const std::vector<prob> &probs);

template<typename IT, typename P>
bool simulateThreshold(const belief &startb, const P &pomdp, std::size_t T, IT &policy) {
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

template<typename IT, typename P>
prob quantifyThreshold_s(const belief &startb, const P &pomdp, const std::size_t T, IT &policy,
                         const std::size_t reps) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < reps; ++i) {
        policy.reset();
        if (simulateThreshold<IT, P>(startb, pomdp, T, policy)) {
            ++count;
        }
    }

    return prob(static_cast<double>(count) / static_cast<double>(reps));
}

template<typename IT, typename P>
prob quantifyThreshold_s(const belief &startb, const P &pomdp, const std::size_t T, IT &policy) {
    return quantifyThreshold_s<IT, P>(startb, pomdp, T, policy, 10000);
}

template<typename IT, typename P>
std::vector<prob> quantifyThreshold_m(belief &&startb, const P &pomdp, const std::size_t T, IT &policy) {
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

template<typename IT, typename P>
std::size_t simulateExpectation(const belief &startb, const P &pomdp, IT &policy) {
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

template<typename IT, typename P>
double quantifyExpectation_s(const belief &startb, const P &pomdp, IT &policy, const std::size_t reps) {
    policy.reset();
    double avg = simulateExpectation<IT, P>(startb, pomdp, policy);

    for (std::size_t i = 2; i <= reps; ++i) {
        policy.reset();
        const double newVal = simulateExpectation<IT, P>(startb, pomdp, policy);
        avg = avg + (newVal - avg) / static_cast<double>(i);
    }

    return avg;
}

template<typename IT, typename P>
double quantifyExpectation_s(const belief &startb, const P &pomdp, IT &policy) {
    return quantifyExpectation_s<IT, P>(startb, pomdp, policy, 10000);
}

// Calculates after how many actions taken with pomdp, startb and policy we will have probability at least threshold,
// that we have reached end, returns pair, where first is the number of steps it took and second is the expectation of
// how many steps it took with the condition that, the probability is at most threshold
template<typename IT, typename P>
std::pair<std::size_t, double> quantify_steps_until_threshold_m(belief &&startb, const P &pomdp, IT &policy, prob threshold) {
    if (threshold == prob(0)) {
        return {0,0};
    }

    auto inter_bel = std::move(startb);
    double expectation = 0;
    prob last_prob = pomdp.ProbAtEnd(inter_bel);

    std::size_t res = 0;
    for (;!pomdp.IsAtEndProb(inter_bel, threshold); ++res) {
        action action = *policy;
        inter_bel = pomdp.UpdateBelief(std::move(inter_bel), action);
        prob new_prob = pomdp.ProbAtEnd(inter_bel);
        expectation += (res + 1) * (new_prob - last_prob);
        last_prob = new_prob;
        ++policy;
    }

    return {res, expectation / threshold};
}
#endif //INC_2026_JAROS_QUANTIFIER_HPP
