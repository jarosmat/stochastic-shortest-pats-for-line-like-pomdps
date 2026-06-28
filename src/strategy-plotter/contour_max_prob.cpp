#include "matplotlibcpp.h"
#include "plotting.hpp"
namespace plt = matplotlibcpp;

int main() {
    constexpr int rows = 9;
    constexpr int cols = 9;
    constexpr int colors = 1;

    std::vector<float> buffer(rows * cols * colors);

    for (int j = 0; j < rows; ++j) {
        for (int i = 0; i < cols; ++i) {
            const std::size_t index = j * cols + i;
            std::vector probs_to_plot{prob{(j + 1) * 0.1}, prob{(i + 1) * 0.1}};
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
    for (auto i{0}; i < 9; ++i) {
        ticks.push_back(i);
    }
    const std::vector<std::string> labels = {"0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"};

    plt::xticks(ticks, labels);
    plt::yticks(ticks, labels);
    plt::xlabel("Second prob");
    plt::ylabel("First prob");
    plt::show();

    return 0;
}
