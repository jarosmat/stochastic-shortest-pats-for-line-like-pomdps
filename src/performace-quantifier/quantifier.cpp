#include "quantifier.hpp"

#include <random>

std::size_t sample_index(const std::vector<prob> &probs) {
    std::vector<prob> cdf(probs.size());
    auto sum = prob(0);
    for (std::size_t i = 0; i < probs.size(); ++i) {
        sum+= probs[i];
        cdf[i] = sum;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.0, 1.0);

    const auto rnum = prob(dist(gen));

    std::size_t i = 0;
    while (rnum > cdf[i] && i < probs.size()) {
        ++i;
    }

    return i;
}
