// #include <catch2/catch_test_macros.hpp>
#include "quantifier.hpp"
#include "periodicPolicy.hpp"
#include <iostream>

int main() {
    double p = static_cast<double>(1) / 2;
    prob pp = prob(p);
    const std::vector<prob> vals = {prob(p), prob(p)};
    lineLikePomdp pomdp(vals);

    // periodicPolicy policy({0,0,0,0,0,1,1,1,1,1});
    periodicPolicy policy({0, 0, 0, 0, 1, 1, 1, 1, 1, 1});
    // periodicPolicy policy({0,1,0,1,0,1,0,1,0,1});
    // periodicPolicy policy({0, 0, 0, 1, 1, 1, 0, 0, 1, 1});

    belief startB = belief({prob(1), prob(0), prob(0)});

    auto res = quantifyThreshold_s(startB, pomdp, 10, policy);

    // for (auto &&a: res) {
    std::cout << res << std::endl;
    // }
    return 0;
}
