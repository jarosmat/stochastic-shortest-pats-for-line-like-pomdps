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
        if (auto [pol_n, bel_n, exp_n, steps_n] = std::move(queue.front());
            pomdp.IsAtEndProb(bel_n, threshold) && exp_n / threshold < res.back().expectation) {
            res.emplace_back(std::move(pol_n), std::move(bel_n), exp_n / threshold, stps);
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
    return exp + n_steps * (threshold - at_end_p);
}

// returns true iff minimal result expectation can be achieved from [exp], that was achieved after [n_steps] - 1 steps,
// with [threshold] and probability, that we have reached the end being [at_end_p], is greater, than already found expectation
// [exp_to_comp]
// in short it just computes the amount, that needs to be added to last state probability, before there is threshold probability
// and multiplies it with [n_steps] and checks if it is greater than [exp_to_comp]
// exp is expected to not being normalized with threshold and return value is not normalized with threshold as well
bool too_high_exp(const double exp_to_comp, const double exp, const prob at_end_p, const prob threshold,
                  const std::size_t n_steps) {
    return exp_to_comp <= min_exp(exp, at_end_p, threshold, n_steps) / threshold;
}

bool valid_bel(const belief &bel) {
    return std::to_string(static_cast<double>(bel[0])) != std::to_string(0.5) || std::to_string(
               static_cast<double>(bel[2])) != std::to_string(0.5);
}

std::vector<queue_e> plot_percentile_max(const prob p1, const prob p2, const prob threshold,
                                         std::size_t time_cutoff = 10) {
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
            if (auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
                elapsed >= time_cutoff) {
                std::cout << "Probabilities: " << p1 << " and " << p2 << ", took: " << stps << '\n';
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
        } else if ((res.empty() || res.back().expectation > exp / threshold) && valid_bel(bel)) {
            res.emplace_back(std::move(pol), std::move(bel), exp / threshold, stps);
        }
    }


    return res;
}

template<char I>
void rem_suffix(std::string &s) {
    while (!s.empty() && s.back() == I) {
        s.pop_back();
    }
}

template<typename F>
class filler {
    std::unordered_map<std::string, std::size_t> strategy_map_;
    std::size_t curr_strategy_index_{0};
    std::vector<std::string> unique_labels_;
    F transformer_;

public:
    float get_index(std::string &&strat) {
        transformer_(strat);
        if (const auto lookup = strategy_map_.find(strat); lookup != strategy_map_.end()) {
            return static_cast<float>(lookup->second);
        }
        strategy_map_.insert({strat, curr_strategy_index_});
        unique_labels_.push_back(std::move(strat));
        return static_cast<float>(curr_strategy_index_++);
    }
};

struct plain_filler {
    std::unordered_map<std::string, std::size_t> strategy_map;
    std::size_t curr_strategy_index{0};
    std::vector<std::string> unique_labels;

    float get_index(std::string &&strat) {
        if (const auto lookup = strategy_map.find(strat); lookup != strategy_map.end()) {
            return static_cast<float>(lookup->second);
        }
        strategy_map.insert({strat, curr_strategy_index});
        unique_labels.push_back(std::move(strat));
        return static_cast<float>(curr_strategy_index++);
    }
};


struct trail_b_less_filler {
    std::unordered_map<std::string, std::size_t> strategy_map;
    std::size_t curr_strategy_index{0};
    std::vector<std::string> unique_labels;

    float get_index(std::string &&strat) {
        rem_suffix<s_instr>(strat);
        if (const auto lookup = strategy_map.find(strat); lookup != strategy_map.end()) {
            return static_cast<float>(lookup->second);
        }
        strategy_map.insert({strat, curr_strategy_index});
        unique_labels.push_back(std::move(strat));
        return static_cast<float>(curr_strategy_index++);
    }
};

struct string_len_filler {
    static constexpr std::size_t init_size{4};
    std::unordered_map<std::string, std::size_t> strategy_map;
    std::size_t curr_strategy_index{init_size};
    std::vector<std::string> unique_labels;


    string_len_filler() : strategy_map(std::move(create_init_map(std::make_index_sequence<init_size>()))),
                          unique_labels(std::move(create_init_vec(std::make_index_sequence<init_size>()))) {
    }

    float get_index(std::string &&strat) {
        const auto size{strat.size()};
        update_labels(size);
        return static_cast<float>(size);
    }

private:
    void update_labels(const std::size_t n) {
        unique_labels.reserve(n + 1);
        for (auto i{unique_labels.size()}; i <= n; ++i) {
            unique_labels.emplace_back(std::move(std::to_string(i)));
            strategy_map.emplace(std::move(std::to_string(i)), i);
        }
        curr_strategy_index = unique_labels.size();
    }


    template<std::size_t ... InitVals>
    static std::vector<std::string> create_init_vec(std::index_sequence<InitVals...>) {
        std::vector<std::string> res;
        res.reserve(sizeof...(InitVals));
        (res.push_back(std::move(std::to_string(InitVals))), ...);

        return res;
    }

    template<std::size_t ... InitVals>
    static std::unordered_map<std::string, std::size_t> create_init_map(std::index_sequence<InitVals...>) {
        std::unordered_map<std::string, std::size_t> res;
        res.reserve(sizeof...(InitVals));
        (res.insert({std::to_string(InitVals), InitVals}), ...);

        return res;
    }
};

struct string_len_filler_trail_b_less {
    static constexpr std::size_t init_size{4};
    std::unordered_map<std::string, std::size_t> strategy_map;
    std::size_t curr_strategy_index{init_size};
    std::vector<std::string> unique_labels;


    string_len_filler_trail_b_less() : strategy_map(std::move(create_init_map(std::make_index_sequence<init_size>()))),
                                       unique_labels(
                                           std::move(create_init_vec(std::make_index_sequence<init_size>()))) {
    }

    float get_index(std::string &&strat) {
        rem_suffix<s_instr>(strat);
        const auto size{strat.size()};
        update_labels(size);
        return static_cast<float>(size);
    }

private:
    void update_labels(const std::size_t n) {
        unique_labels.reserve(n + 1);
        for (auto i{unique_labels.size()}; i <= n; ++i) {
            unique_labels.emplace_back(std::move(std::to_string(i)));
            strategy_map.emplace(std::move(std::to_string(i)), i);
        }
        curr_strategy_index = unique_labels.size();
    }


    template<std::size_t ... InitVals>
    static std::vector<std::string> create_init_vec(std::index_sequence<InitVals...>) {
        std::vector<std::string> res;
        res.reserve(sizeof...(InitVals));
        (res.push_back(std::move(std::to_string(InitVals))), ...);

        return res;
    }

    template<std::size_t ... InitVals>
    static std::unordered_map<std::string, std::size_t> create_init_map(std::index_sequence<InitVals...>) {
        std::unordered_map<std::string, std::size_t> res;
        res.reserve(sizeof...(InitVals));
        (res.insert({std::to_string(InitVals), InitVals}), ...);

        return res;
    }
};

template<typename T>
    requires (!std::is_reference_v<T>)
std::vector<float> fill_buffer(T &indexer, const std::size_t prob1_count, const std::size_t prob2_count,
                               const float grain,
                               const float start_prob1, const float start_prob2, const prob threshold,
                               auto plotter_func) {
    std::vector<float> buffer(prob1_count * prob2_count);
    for (int j{0}; j < prob1_count; ++j) {
        for (int i{0}; i < prob2_count; ++i) {
            const std::size_t index{j * prob2_count + i};
            std::vector probs_to_plot{
                prob{static_cast<float>(j) * grain + start_prob1}, prob{static_cast<float>(i) * grain + start_prob2}
            };
            auto vals{plotter_func(probs_to_plot[0], probs_to_plot[1], threshold)};
            auto strategy{vals.back().policy};
            const float value{indexer.get_index(std::move(strategy))};
            buffer[index] = value;
        }
    }

    return buffer;
}

std::vector<double> get_ticks(const float grain, const float start_prob, const uint count) {
    std::vector<double> res;
    res.reserve(9);
    const double len{1 / (grain * 10)};
    for (auto i{1}, j{1}; i <= 9 && res.size() < count; ++i) {
        if (static_cast<float>(0.1 * i) >= start_prob) {
            res.push_back(static_cast<double>(j) * len - 1);
            ++j;
        }
    }

    return res;
}

std::vector<std::string> get_labels(const float start_prob, const uint count) {
    const std::vector<std::string> labels_strings = {"0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"};
    std::vector<std::string> res;
    res.reserve(9);

    for (auto i{1u}; i <= 9 && res.size() < count; ++i) {
        if (static_cast<float>(0.1 * i) >= start_prob) {
            res.push_back(labels_strings[i - 1]);
        }
    }


    return res;
}

enum struct display_vals {
    string_val,
    string_len,
    invalid,
};

enum struct quantities {
    min_expectation,
    min_str_len,
    invalid
};


struct program_params {
    uint prob1_count = 9;
    uint prob2_count = 9;
    float grain = 0.1f;
    float start_prob1 = 0.1f;
    float start_prob2 = 0.1f;
    uint colors = 1;
    double threshold_val = 0.5;
    display_vals to_show = display_vals::string_val;
    quantities to_optimize = quantities::min_expectation;
    bool trim_b = false;
};

enum struct parse_errors {
    invalid_option,
    invalid_option_value,
    missing_option_value
};

quantities parse_quantity(const std::string &val) {
    const std::string min_exp{"min_exp"};
    const std::string min_str_len{"min_str_len"};

    if (val == min_exp) {
        return quantities::min_expectation;
    }
    if (val == min_str_len) {
        return quantities::min_str_len;
    }

    return quantities::invalid;
}

display_vals parse_display_vals(const std::string &val) {
    const std::string string_val{"string_val"};
    const std::string string_len{"string_len"};

    if (val == string_val) {
        return display_vals::string_val;
    }
    if (val == string_len) {
        return display_vals::string_len;
    }

    return display_vals::invalid;
}


std::variant<program_params, parse_errors> parse_arguments(const std::vector<std::string> &args) {
    program_params params;


    const std::string prob1_count_str = "--prob1-count";
    const std::string prob2_count_str = "--prob2-count";
    const std::string grain_str = "--grain";
    const std::string start_prob1_str = "--start-prob1";
    const std::string start_prob2_str = "--start-prob2";
    const std::string colors_str = "--colors";
    const std::string threshold_val_str = "--threshold-val";
    const std::string to_display_str = "--display_val";
    const std::string quantity_str = "--quantity";
    const std::string trim_b_str = "--trim-b";

    for (auto i{0uz}; i < args.size(); ++i) {
        if (const std::string &arg{args[i]}; arg == prob1_count_str && i + 1 < args.size()) {
            params.prob1_count = std::stoul(args[++i]);
        } else if (arg == prob2_count_str && i + 1 < args.size()) {
            params.prob2_count = std::stoul(args[++i]);
        } else if (arg == grain_str && i + 1 < args.size()) {
            params.grain = std::stof(args[++i]);
        } else if (arg == start_prob1_str && i + 1 < args.size()) {
            params.start_prob1 = std::stof(args[++i]);
        } else if (arg == start_prob2_str && i + 1 < args.size()) {
            params.start_prob2 = std::stof(args[++i]);
        } else if (arg == colors_str && i + 1 < args.size()) {
            params.colors = std::stoul(args[++i]);
        } else if (arg == threshold_val_str && i + 1 < args.size()) {
            params.threshold_val = std::stod(args[++i]);
        } else if (arg == to_display_str && i + 1 < args.size()) {
            const auto parse_res{parse_display_vals(args[++i])};
            if (parse_res == display_vals::invalid) {
                return parse_errors::invalid_option_value;
            }
            params.to_show = parse_res;
        } else if (arg == quantity_str && i + 1 < args.size()) {
            const auto parse_res{parse_quantity(args[++i])};
            if (parse_res == quantities::invalid) {
                return parse_errors::invalid_option_value;
            }
            params.to_optimize = parse_res;
        } else if (arg == trim_b_str) {
            params.trim_b = true;
        } else {
            return parse_errors::invalid_option;
        }
    }

    return params;
}

int main(const int argc, char *argv[]) {
    const std::vector<std::string> args(argv + 1, argv + argc);
    const auto parse_res = parse_arguments(args);

    if (parse_res.index() == 1) {
        switch (std::get<parse_errors>(parse_res)) {
            case parse_errors::invalid_option:
                std::cerr << "Invalid option" << std::endl;
                break;
            case parse_errors::invalid_option_value:
                std::cerr << "Invalid option value" << std::endl;
                break;
            case parse_errors::missing_option_value:
                std::cerr << "Option value not specified" << std::endl;
                break;
        }
        return -1;
    }
    auto [
        prob1_count,
        prob2_count,
        grain,
        start_prob1,
        start_prob2,
        colors,
        threshold_val,
        to_show,
        to_optimize,
        trim_b
    ]
    {
        std::get<0>(parse_res)
    };
    const prob threshold{threshold_val};


    plain_filler filler;
    const std::vector buffer{
        fill_buffer(filler, prob1_count, prob2_count, grain, start_prob1, start_prob2, threshold,
                    [](const prob p1, const prob p2, const prob thr) {
                        return plot_percentile_max(p1, p2, thr);
                    })
    };
    auto &[strategy_map, curr_strategy_index, unique_labels]{filler};


    std::map<std::string, std::string> keywords;
    keywords["cmap"] = "turbo";
    keywords["interpolation"] = "none";
    keywords["vmin"] = "0";
    keywords["vmax"] = std::to_string(curr_strategy_index - 1);

    PyObject *plot_output = nullptr;
    plt::imshow(buffer.data(), static_cast<int>(prob1_count), static_cast<int>(prob2_count), static_cast<int>(colors),
                keywords, &plot_output);
    plt::colorbar(plot_output);

    if (PyObject *pyplot_module = PyImport_ImportModule("matplotlib.pyplot")) {
        if (PyObject *gci_func = PyObject_GetAttrString(pyplot_module, "gci")) {
            if (PyObject *current_image = PyObject_CallNoArgs(gci_func)) {
                if (PyObject *cbar = PyObject_GetAttrString(current_image, "colorbar")) {
                    PyObject *ticks_py = PyList_New(static_cast<long>(curr_strategy_index));
                    PyObject *labels_py = PyList_New(static_cast<long>(curr_strategy_index));

                    for (int k = 0; k < curr_strategy_index; ++k) {
                        PyList_SetItem(ticks_py, k, PyFloat_FromDouble(k));
                        PyList_SetItem(labels_py, k, PyString_FromString(unique_labels[k].c_str()));
                    }

                    PyObject *res1 = PyObject_CallMethod(cbar, "set_ticks", "O", ticks_py);
                    PyObject *res2 = PyObject_CallMethod(cbar, "set_ticklabels", "O", labels_py);

                    if (res1)
                        Py_DECREF(res1);
                    if (res2)
                        Py_DECREF(res2);
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

    const std::vector ticks_prob_1{get_ticks(grain, start_prob1, prob1_count)};
    const std::vector ticks_prob_2{get_ticks(grain, start_prob2, prob2_count)};
    const std::vector labels_prob_1{get_labels(start_prob1, prob1_count)};
    const std::vector labels_prob_2{get_labels(start_prob2, prob2_count)};


    plt::xticks(ticks_prob_2, labels_prob_2);
    plt::yticks(ticks_prob_1, labels_prob_1);
    plt::xlabel("Second prob");
    plt::ylabel("First prob");
    plt::title(
        "Percentile: " + std::to_string(threshold_val) + ", podle delek strategii, prioritizujeme delku strategie");
    plt::show();
    return 0;
}
