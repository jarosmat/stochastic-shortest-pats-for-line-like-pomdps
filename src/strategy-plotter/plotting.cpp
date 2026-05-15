#include "plotting.hpp"


// returns periodic policy, where there is counts.size() instructions and
// for instruction i, there are counts[i] instructions in sequence
periodicPolicy getPolicy(const std::vector<std::size_t> &counts) {
    std::vector<std::size_t> res;

    for (std::size_t action = 0; action < counts.size(); ++action) {
        const std::size_t a_count = counts[action];
        for (std::size_t i = 0; i < a_count; ++i) {
            res.push_back(action);
        }
    }

    return periodicPolicy(std::move(res));
}

// returns vector of size T - 1, where result[i] is equal to probability of getting to final state after T actions
// with policy, where there is i + 1 A instructions and T - (i + 1) B instructions
// TODO implement for more tha two probabilities, the policy generation must be updated
std::vector<prob> plot_probs(std::vector<prob> &&probs, const std::size_t T) {
    const auto pomdp = lineLikePomdp(std::move(probs));
    std::vector start_bel(pomdp.States(), prob(0));
    start_bel[0] = prob(1);

    std::vector<prob> res(T - 1);

    for (std::size_t i = 1; i < T; ++i) {
        std::vector<std::size_t> action_counts;
        action_counts.push_back(i);
        action_counts.push_back(T - i);
        auto policy = getPolicy(action_counts);

        auto res_plot = quantifyThreshold_m(belief(start_bel), pomdp, T, policy);

        res[i - 1] = res_plot[0];
    }

    return res;
}
