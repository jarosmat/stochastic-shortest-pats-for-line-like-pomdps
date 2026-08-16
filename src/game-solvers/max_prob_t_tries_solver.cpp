#include "max_prob_t_tries_solver.hpp"
#include "expandedPolicy.hpp"
#include "quantifier.hpp"
#include <algorithm>
#include "combinations_with_repetition.hpp"


std::pair<expanded_policy, prob> max_prob_t_tries_solver::solve(
    const lineLikePomdp &pomdp, std::size_t t, const belief &start_b) {
    if (start_b.bel.empty()) {
        return {expanded_policy({}, 0), prob(0)};
    }
    if (start_b.bel.size() == 1) {
        return {expanded_policy({}, 0), prob(1)};
    }

    auto max_lambda = [](const auto &left, const auto &right) {
        return left.second < right.second;
    };

    auto diff_mvs{start_b.bel.size() - 1};
    if (t < diff_mvs) {
        return {expanded_policy({}, diff_mvs), prob(0)};
    }
    auto transform_lambda = [&pomdp, &start_b, t, diff_mvs](const auto &comb) -> std::pair<expanded_policy, prob> {
        auto pol{expanded_policy(comb, diff_mvs - 1)};
        auto start_bel{start_b};
        auto res{quantifyThreshold_m(std::move(start_bel), pomdp, t, pol).back()};
        return {std::move(pol), res};
    };

    auto mvs = std::ranges::views::iota(0uz, diff_mvs);
    auto results = mvs | combinations_with_repetition(t - diff_mvs) | std::views::transform(transform_lambda);

    return std::ranges::max(results, max_lambda);
}
