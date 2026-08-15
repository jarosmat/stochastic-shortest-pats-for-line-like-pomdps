#include "min_moves_before_threshold_solver.hpp"

#include <queue>

namespace {
    struct queue_e {
        std::vector<std::size_t> used_counts;
        belief bel;
        double expectation;
        std::size_t steps;
    };
}

static void add_new_moves(const lineLikePomdp &pomdp, std::queue<queue_e> &queue) {
    auto [counts, bel, exp, steps] = std::move(queue.front());
    queue.pop();
    const auto moves{bel.bel.size() - 1};

    for (auto i{0uz}; i < moves; ++i) {
        auto n_counts{counts};
        ++n_counts[i];
        auto prob_end{pomdp.ProbAtEnd(bel)};
        belief n_bel{pomdp.UpdateBelief(bel, i)};

        const auto new_steps{steps + 1};
        double new_exp{exp + new_steps * (pomdp.ProbAtEnd(n_bel) - prob_end)};

        queue.emplace(std::move(n_counts), std::move(n_bel), new_exp, new_steps);
    }
}

static std::vector<queue_e> get_best_counts(const lineLikePomdp &pomdp, const prob threshold, const belief &start_b) {
    const auto moves{start_b.bel.size()};
    std::queue<queue_e> queue;
    queue.emplace(std::vector<std::size_t>(moves, 0), start_b, 0, 0);

    while (!pomdp.IsAtEndProb(queue.front().bel, threshold)) {
        add_new_moves(pomdp, queue);
    }


    auto [pol, bel, exp, stps] = std::move(queue.front());
    queue.pop();
    std::vector<queue_e> res;
    res.emplace_back(std::move(pol), std::move(bel), exp / threshold, stps);

    while (!queue.empty() && queue.front().steps == stps) {
        if (auto [used_n, bel_n, exp_n, steps_n] = std::move(queue.front());
            pomdp.IsAtEndProb(bel_n, threshold) && exp_n / threshold < res.back().expectation) {
            res.emplace_back(std::move(used_n), std::move(bel_n), exp_n / threshold, stps);
        }
        queue.pop();
    }

    return res;
}

std::pair<periodicPolicy, std::size_t> min_moves_before_threshold_solver::solve(
    const lineLikePomdp &pomdp, const prob threshold,
    const belief &start_b) {
    auto best_counts{get_best_counts(pomdp, threshold, start_b)};

    
}
