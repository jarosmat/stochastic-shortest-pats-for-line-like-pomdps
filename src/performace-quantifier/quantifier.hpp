#ifndef INC_2026_JAROS_QUANTIFIER_HPP
#define INC_2026_JAROS_QUANTIFIER_HPP

#include "prob.hpp"
#include "belief.hpp"
#include "lineLikePomdp.hpp"

//this function is operating under the assumption, that sum of elements of probs is 1
std::size_t sample_index(const std::vector<prob> &probs);

template<typename It, typename P>
bool simulate_threshold(const belief &startb, const P &pomdp, const std::size_t t, It &policy) {
    auto state = sample_index(startb.bel);
    if (pomdp.is_final_state(state)) {
        return true;
    }
    for (std::size_t i = 0; i < t; ++i) {
        const action action = *policy;
        ++policy;
        auto nextb = pomdp.simulate_action(action, state);

        state = sampleIndex(nextb.bel);

        if (pomdp.is_final_state(state)) {
            return true;
        }
    }
    return false;
}

template<typename It, typename P>
prob quantify_threshold_s(const belief &startb, const P &pomdp, const std::size_t t, It &policy,
                         const std::size_t reps) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < reps; ++i) {
        policy.reset();
        if (simulateThreshold<It, P>(startb, pomdp, t, policy)) {
            ++count;
        }
    }

    return prob(static_cast<double>(count) / static_cast<double>(reps));
}

template<typename It, typename P>
prob quantify_threshold_s(const belief &startb, const P &pomdp, const std::size_t t, It &policy) {
    return quantifyThreshold_s<It, P>(startb, pomdp, t, policy, 10000);
}

template<typename It, typename P>
std::vector<prob> quantify_threshold_m(belief &&startb, const P &pomdp, const std::size_t t, It &policy) {
    if (t == 0) {
        return pomdp.get_final_states_probs(std::move(startb));
    }
    action action = *policy;
    auto inter_res = pomdp.update_belief(std::move(startb), action);
    ++policy;

    for (std::size_t i = 1; i < t; ++i) {
        action = *policy;
        inter_res = pomdp.update_belief(std::move(inter_res), action);
        ++policy;
    }

    return pomdp.get_final_states_probs(std::move(inter_res));
}

template<typename It, typename P>
std::size_t simulate_expectation(const belief &startb, const P &pomdp, It &policy) {
    std::size_t count = 0;
    auto state = sample_index(startb.bel);

    while (!pomdp.is_final_state(state)) {
        const action action = *policy;
        ++policy;
        auto nextb = pomdp.simulate_action(action, state);
        ++count;

        state = sample_index(nextb.bel);
    }

    return count;
}

template<typename It, typename P>
double quantify_expectation_s(const belief &startb, const P &pomdp, It &policy, const std::size_t reps) {
    policy.reset();
    double avg = simulate_expectation<It, P>(startb, pomdp, policy);

    for (std::size_t i = 2; i <= reps; ++i) {
        policy.reset();
        const double new_val = simulate_expectation<It, P>(startb, pomdp, policy);
        avg = avg + (new_val - avg) / static_cast<double>(i);
    }

    return avg;
}

template<typename It, typename P>
double quantify_expectation_s(const belief &startb, const P &pomdp, It &policy) {
    return quantify_expectation_s<It, P>(startb, pomdp, policy, 10000);
}

// Calculates after how many actions taken with pomdp, startb and policy we will have probability at least threshold,
// that we have reached end, returns pair, where first is the number of steps it took and second is the expectation of
// how many steps it took with the condition that, the probability is at most threshold
template<typename It, typename P>
std::pair<std::size_t, double> quantify_steps_until_threshold_m(belief &&startb, const P &pomdp, It &policy, prob threshold) {
    if (threshold == prob(0)) {
        return {0,0};
    }

    auto inter_bel = std::move(startb);
    double expectation = 0;
    prob last_prob = pomdp.prob_at_end(inter_bel);

    std::size_t res = 0;
    for (;!pomdp.is_at_end_prob(inter_bel, threshold); ++res) {
        action action = *policy;
        inter_bel = pomdp.update_belief(std::move(inter_bel), action);
        prob new_prob = pomdp.prob_at_end(inter_bel);
        expectation += (res + 1) * (new_prob - last_prob);
        last_prob = new_prob;
        ++policy;
    }

    return {res, expectation / threshold};
}
#endif //INC_2026_JAROS_QUANTIFIER_HPP
