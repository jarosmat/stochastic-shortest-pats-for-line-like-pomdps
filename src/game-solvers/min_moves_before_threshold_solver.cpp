#include "min_moves_before_threshold_solver.hpp"
#include "permutations_with_repetition.hpp"
#include "quantifier.hpp"
#include <queue>

namespace {
    struct queue_e {
        std::vector<std::size_t> used_counts;
        belief bel;
        double expectation;
        std::size_t steps;
    };
}

static void add_new_moves(const line_like_pomdp &pomdp, std::queue<queue_e> &queue) {
    auto [counts, bel, exp, steps] = std::move(queue.front());
    queue.pop();
    const auto moves{bel.bel.size() - 1};

    for (auto i{0uz}; i < moves; ++i) {
        auto n_counts{counts};
        ++n_counts[i];
        auto prob_end{pomdp.prob_at_end(bel)};
        belief n_bel{pomdp.update_belief(bel, i)};

        const auto new_steps{steps + 1};
        double new_exp{exp + new_steps * (pomdp.prob_at_end(n_bel) - prob_end)};

        queue.emplace(std::move(n_counts), std::move(n_bel), new_exp, new_steps);
    }
}

static queue_e get_best_counts(const line_like_pomdp &pomdp, const prob threshold, const belief &start_b) {
    const auto moves{start_b.bel.size()};
    std::queue<queue_e> queue;
    queue.emplace(std::vector<std::size_t>(moves, 0), start_b, 0, 0);

    while (!pomdp.is_at_end_prob(queue.front().bel, threshold)) {
        add_new_moves(pomdp, queue);
    }

    auto res{std::move(queue.front())};
    const auto stps{res.steps};
    res.expectation = res.expectation / threshold;
    queue.pop();

    while (!queue.empty() && queue.front().steps == stps) {
        if (auto &[used_n, bel_n, exp_n, steps_n]{queue.front()};
            pomdp.is_at_end_prob(bel_n, threshold) && exp_n / threshold < res.expectation) {
            res = std::move(queue.front());
            res.expectation = res.expectation / threshold;
        }
        queue.pop();
    }

    return res;
}

static std::vector<std::pair<std::size_t, std::size_t> >
get_counts_transformed(const std::vector<std::size_t> &counts) {
    std::vector<std::pair<std::size_t, std::size_t> > res;
    const auto size_c{counts.size()};
    res.reserve(size_c);
    for (auto i{0uz}; i < size_c; ++i) {
        res.emplace_back(i, counts[i]);
    }

    return res;
}

std::pair<periodic_policy, std::pair<std::size_t, double> > min_moves_before_threshold_solver::solve(
    const line_like_pomdp &pomdp, const prob threshold,
    const belief &start_b) {
    auto [counts, _, exp, steps]{get_best_counts(pomdp, threshold, start_b)};
    auto transformed{get_counts_transformed(counts)};

    auto transformer = [&start_b, &pomdp, threshold]<typename P>(
        P &&perm) -> std::pair<periodic_policy, std::pair<std::size_t, double> > {
        auto pol{periodic_policy(std::forward<P>(perm))};
        auto bel{start_b};
        auto res{quantify_steps_until_threshold_m(std::move(bel), pomdp, pol, threshold)};
        return {std::move(pol), res};
    };

    auto max_lambda = [](const auto &left, const auto &right) {
        if (left.second.first == right.second.first) {
            return left.second.second > right.second.second;
        }
        return left.second.first > right.second.first;
    };

    auto results = transformed | permutations_with_repetition{} | std::views::transform(transformer);
    auto [pol, pol_metrics]{std::ranges::max(results, max_lambda)};

    return {std::move(pol), pol_metrics};
}
