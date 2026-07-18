#include <queue>

#include "matplotlibcpp.h"
#include "plotting.hpp"
#include "quantifier.hpp"
#include <chrono>

namespace plt = matplotlibcpp;

constexpr char s_instr{'B'};

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
        auto [pol, bel, exp, steps] = std::move(queue.front());
        queue.pop();
        auto pol_a{pol};
        pol_a.push_back('A');
        auto pol_b{std::move(pol)};
        pol_b.push_back('B');

        auto prob_end{pomdp.ProbAtEnd(bel)};

        belief n_bel_a{pomdp.UpdateBelief(bel, 0)};
        belief n_bel_b{pomdp.UpdateBelief(std::move(bel), 1)};

        const auto new_steps{steps + 1};
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

// Computes minimal expectation that may be achieved with current probability [at_end_prob] exp amassed after [n_steps] - 1
// and [threshold]
// in short it just computes the amount, that needs to be added to last state probability, before there is threshold probability
// and multiplies it with [n_steps]
// exp is expected to not being normalized with threshold and return value is not normalized with threshold as well
double min_exp(const double exp, const prob at_end_p, const prob threshold, const std::size_t n_steps) {
    if (at_end_p >= threshold) {
        return exp;
    }
    return  exp + n_steps * (threshold - at_end_p);
}

// returns true iff minimal result expectation can be achieved from [exp], that was achieved after [n_steps] - 1 steps,
// with [threshold] and probability, that we have reached the end being [at_end_p], is greater, than already found expectation
// [exp_to_comp]
// in short it just computes the amount, that needs to be added to last state probability, before there is threshold probability
// and multiplies it with [n_steps] and checks if it is greater than [exp_to_comp]
// exp is expected to not being normalized with threshold and return value is not normalized with threshold as well
bool too_high_exp(const double exp_to_comp ,const double exp, const prob at_end_p, const prob threshold, const std::size_t n_steps) {
    return  exp_to_comp <= min_exp(exp, at_end_p, threshold, n_steps) / threshold;
}

std::vector<queue_e> plot_percentile_max(const prob p1, const prob p2, const prob threshold, std::size_t time_cutoff = 10) {
    auto start = std::chrono::steady_clock::now();
    const lineLikePomdp pomdp({p1, p2});

    std::vector start_v{prob(1), prob(0), prob(0)};
    belief start_b(std::move(start_v));

    std::queue<queue_e> queue;
    queue.emplace("", std::move(start_b), 0, 0);

    std::vector<queue_e> res;
    while (!queue.empty()) {
        auto [pol, bel, exp, stps] = std::move(queue.front());
        queue.pop();
        if (!res.empty()) {
            auto end = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            if (elapsed >= time_cutoff) {
                std::cout << "Probabilities: "  << p1 << " and " << p2 << ", took: " << stps << '\n';
                return res;
            }
        }
        if (!pomdp.IsAtEndProb(bel, threshold)) {
            if (!res.empty() && too_high_exp(res.back().expectation, exp, pomdp.ProbAtEnd(bel), threshold, stps + 1)) {
                continue;
            }

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
        else if (res.empty()) {
            res.emplace_back(std::move(pol), std::move(bel), exp / threshold, stps);
        } else if (res.back().expectation > exp / threshold) {
            res.emplace_back(std::move(pol), std::move(bel), exp / threshold, stps);
        }
    }


    return res;
}

template<char I>
void rem_suffix(std::string& s) {
    while (!s.empty() && s.back() == I) {
        s.pop_back();
    }
}

class plain_filler {
    std::unordered_map<std::string, std::size_t> strategy_map_;
    std::size_t curr_strategy_index_{0};
    std::vector<std::string> unique_labels_;
public:
    float get_index(std::string&& strat) {
        if (const auto lookup = strategy_map_.find(strat); lookup != strategy_map_.end()) {
            return static_cast<float>(lookup->second);
        }
            strategy_map_.insert({strat, curr_strategy_index_});
            unique_labels_.push_back(std::move(strat));
            return static_cast<float>(curr_strategy_index_++);
            // ++curr_strategy_index;
    }
};


class trail_b_less_filler {
    std::unordered_map<std::string, std::size_t> strategy_map_;
    std::size_t curr_strategy_index_{0};
    std::vector<std::string> unique_labels_;
public:
    float get_index(std::string&& strat) {
        rem_suffix<s_instr>(strat);
        if (const auto lookup = strategy_map_.find(strat); lookup != strategy_map_.end()) {
            return static_cast<float>(lookup->second);
        }
        strategy_map_.insert({strat, curr_strategy_index_});
        unique_labels_.push_back(std::move(strat));
        return static_cast<float>(curr_strategy_index_++);
        // ++curr_strategy_index;
    }
};

template<typename T>
std::vector<float> fill_buffer(std::size_t rows, std::size_t cols, float grain, float start_prob, int colors, prob threshold) {
    std::vector<float> buffer(rows * cols * colors);
    T indexer;
    for (int j{0}; j < rows; ++j) {
        for (int i{0}; i < cols; ++i) {
            const std::size_t index{j * cols + i};
            std::vector probs_to_plot{prob{static_cast<float>(j) * grain + start_prob}, prob{static_cast<float>(i) * grain + start_prob}};
            auto vals{plot_percentile_max(probs_to_plot[0], probs_to_plot[1], threshold)};
            auto strategy{vals.back().policy};
            // rem_suffix(strategy);
            const float value{indexer.get_index(std::move(strategy))};
            buffer[index] = value;
        }
    }

    return buffer;
}

int main() {
    constexpr int rows = 9;
    constexpr int cols = 9;
    constexpr float grain = 0.1;
    constexpr float start_prob = 0.1;
    constexpr int colors = 1;
    constexpr double threshold_val{0.5};
    const prob threshold{threshold_val};

    std::vector<float> buffer(rows * cols * colors);
    std::unordered_map<std::string, std::size_t> strategy_map;
    std::size_t curr_strategy_index{0};
    std::vector<std::string> unique_labels;
    for (int j = 0; j < rows; ++j) {
        for (int i = 0; i < cols; ++i) {
            const std::size_t index = j * cols + i;
            std::vector probs_to_plot{prob{static_cast<float>(j) * grain + start_prob}, prob{static_cast<float>(i) * grain + start_prob}};
            auto vals{plot_percentile_max(probs_to_plot[0], probs_to_plot[1], threshold)};
            auto strategy{vals.back().policy};
            rem_suffix(strategy);
            auto lookup = strategy_map.find(strategy);
            if (lookup != strategy_map.end()) {
                buffer[index] = static_cast<float>(lookup->second);
            }
            else {
                strategy_map.insert({strategy, curr_strategy_index});
                unique_labels.push_back(std::move(strategy));
                buffer[index] = static_cast<float>(curr_strategy_index);
                ++curr_strategy_index;
            }
        }
    }

    std::map<std::string, std::string> keywords;
    keywords["cmap"] = "turbo";
    keywords["interpolation"] = "none";
    keywords["vmin"] = "0";
    keywords["vmax"] = std::to_string(curr_strategy_index - 1);

    PyObject *plot_output = nullptr;
    plt::imshow(buffer.data(), rows, cols, colors, keywords, &plot_output);
    plt::colorbar(plot_output);

    PyObject* pyplot_module = PyImport_ImportModule("matplotlib.pyplot");
    if (pyplot_module) {
        PyObject* gci_func = PyObject_GetAttrString(pyplot_module, "gci");
        if (gci_func) {
            PyObject* current_image = PyObject_CallNoArgs(gci_func);
            if (current_image) {
                PyObject* cbar = PyObject_GetAttrString(current_image, "colorbar");
                if (cbar) {
                    PyObject* ticks_py = PyList_New(static_cast<long>(curr_strategy_index));
                    PyObject* labels_py = PyList_New(static_cast<long>(curr_strategy_index));

                    for (int k = 0; k < curr_strategy_index; ++k) {
                        PyList_SetItem(ticks_py, k, PyFloat_FromDouble(k));
                        PyList_SetItem(labels_py, k, PyString_FromString(unique_labels[k].c_str()));
                    }

                    PyObject* res1 = PyObject_CallMethod(cbar, "set_ticks", "O", ticks_py);
                    PyObject* res2 = PyObject_CallMethod(cbar, "set_ticklabels", "O", labels_py);

                    if (res1) Py_DECREF(res1);
                    if (res2) Py_DECREF(res2);
                    Py_DECREF(ticks_py);
                    Py_DECREF(labels_py);
                    Py_DECREF(cbar);
                }
                Py_DECREF(current_image);
            }
            Py_DECREF(gci_func);
        }
        Py_DECREF(pyplot_module);
    }

    std::vector<double> ticks;
    ticks.reserve(9);
    constexpr double len{1 /(grain * 10)};
    for (auto i{1}, j{1}; i <= 9; ++i) {
        if (static_cast<float>(0.1 * i) >= start_prob) {
            ticks.push_back(static_cast<double>(j) * len - 1);
            ++j;
        }
    }

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
    plt::title("Percentile: " + std::to_string(threshold_val) + ", podle delek strategii, prioritizujeme delku strategie");
    plt::show();
    return 0;
}
