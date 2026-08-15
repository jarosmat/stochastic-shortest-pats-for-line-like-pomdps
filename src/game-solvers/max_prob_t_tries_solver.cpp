#include "max_prob_t_tries_solver.hpp"
#include "expandedPolicy.hpp"
#include "quantifier.hpp"
#include <algorithm>
#include "combinations_with_repetition.hpp"


std::pair<expanded_policy, prob> max_prob_t_tries_solver::solve(
    const lineLikePomdp &pomdp, std::size_t t, const belief &bel) {
    auto transform_lambda = [&pomdp, &bel, t](const auto &comb) -> std::pair<expanded_policy, prob> {
        for (auto &&a: comb) {
            std::cout << a << " ";
        }
        std::cout << std::endl;
        auto pol{expanded_policy(comb, 3)};
        auto start_bel{bel};
        auto res{quantifyThreshold_m(std::move(start_bel), pomdp, t, pol).back()};
        return {std::move(pol), res};
    };

    auto max_lambda = [](const auto &left, const auto &right) {
        return left.second < right.second;
    };

    auto diff_mvs{bel.bel.size() - 1};
    if (t < diff_mvs) {
        return {expanded_policy({}, diff_mvs), prob(0)};
    }

    auto mvs = std::ranges::views::iota(0uz, diff_mvs);
    auto results = mvs | combinations_with_repetition(t - diff_mvs) | std::views::transform(transform_lambda);

    return std::ranges::max(results, max_lambda);
}
