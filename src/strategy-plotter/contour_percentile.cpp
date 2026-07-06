#include <queue>

#include "matplotlibcpp.h"
#include "plotting.hpp"

namespace plt = matplotlibcpp;

struct queue_e {
    std::string policy;
    belief bel;
    double expectation;
    std::size_t steps;
};


std::vector<queue_e> plot_percentile(const prob p1, const prob p2, prob threshold) {
    const lineLikePomdp pomdp({p1, p2});

    std::vector start_v{prob(1), prob(0), prob(0)};
    belief start_b(std::move(start_v));

    std::queue<queue_e> queue;
    queue.emplace("", std::move(start_b), 0, 0);

    while (!pomdp.IsAtEndProb(queue.front().bel, threshold)) {
        auto [pol, bel, exp, stps] = std::move(queue.front());
        queue.pop();
        auto pol_a{pol};
        pol_a.push_back('A');
        auto pol_b{std::move(pol)};
        pol_b.push_back('B');

        auto prob_end{pomdp.ProbAtEnd(bel)};

        belief n_bel_a{pomdp.UpdateBelief(bel, 0)};
        belief n_bel_b{pomdp.UpdateBelief(std::move(bel), 1)};

        const auto new_steps{stps + 1};
        double new_exp_a{exp + new_steps * (pomdp.ProbAtEnd(n_bel_a) - prob_end)};
        double new_exp_b{exp + new_steps * (pomdp.ProbAtEnd(n_bel_b) - prob_end)};

        queue.emplace(std::move(pol_a), std::move(n_bel_a), new_exp_a, new_steps);
        queue.emplace(std::move(pol_b), std::move(n_bel_b), new_exp_b, new_steps);
    }


    auto [pol, bel, exp, stps] = std::move(queue.front());
    queue.pop();
    std::vector<queue_e> res;
    res.emplace_back(std::move(pol), std::move(bel), exp / threshold, stps);

    while (!queue.empty() && queue.front().steps == stps) {
        auto [poln, beln, expn, stpsn] = std::move(queue.front());
        if (pomdp.IsAtEndProb(beln, threshold)) {
            res.emplace_back(std::move(poln), std::move(beln), expn / threshold, stps);
        }
        queue.pop();
    }

    return res;
}

int main() {
    constexpr int rows = 4;
    constexpr int cols = 4;
    constexpr float grain = 0.1;
    constexpr float start_prob = 0.6;
    constexpr int colors = 1;

    std::vector<float> buffer(rows * cols * colors);

    for (int j = 0; j < rows; ++j) {
        for (int i = 0; i < cols; ++i) {
            const std::size_t index = j * cols + i;
            std::vector probs_to_plot{prob{static_cast<float>(j) * grain + start_prob}, prob{static_cast<float>(i) * grain + start_prob}};
            auto res = plot_probs(std::move(probs_to_plot), 10);
            const auto max_prob = max(res);
            buffer[index] = static_cast<float>(max_prob + 1);
        }
    }

    PyObject *plot_output = nullptr;
    plt::imshow(buffer.data(), rows, cols, colors, {}, &plot_output);

    plt::colorbar(plot_output);
    std::vector<double> ticks;
    ticks.reserve(9);
    constexpr auto len{1 /(grain * 10)};
    for (auto i{1}, j{1}; i <= 9; ++i) {
        std::cout << i << std::endl;
        std::cout << 0.1*i << " | " << start_prob << " | " << (static_cast<float>(0.1 * i) >= start_prob) << std::endl;
        if (static_cast<float>(0.1 * i) >= start_prob) {
            ticks.push_back(j * len - 1);
            ++j;
        }
    }

    for (auto && a : ticks) {
        std::cout << a << " ";
    }
    std::cout << std::endl;


    const std::vector<std::string> labels_strings = {"0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"};
    std::vector<std::string> labels;
    labels.reserve(9);

    for (auto i{9 - ticks.size() + 1}; i <= 9; ++i) {
        labels.push_back(labels_strings[i - 1]);
    }


    plt::xticks(ticks, labels);
    plt::yticks(ticks, labels);
    plt::xlabel("Second prob");
    plt::ylabel("First prob");
    plt::show();
    return 0;
}
