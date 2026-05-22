// #include <catch2/catch_test_macros.hpp>
#include "quantifier.hpp"
#include "periodicPolicy.hpp"
#include <iostream>
#include "lineLikePomdp.hpp"

int main() {
    double p = static_cast<double>(1) / 2;
    prob pp = prob(p);
    const std::vector<prob> vals = {prob(p), prob(p)};
    lineLikePomdp pomdp(vals);

    // periodicPolicy policy({0,0,0,0,0,1,1,1,1,1});
    // periodicPolicy policy({0, 0, 0, 0, 0, 1, 1, 1, 1, 1});
    periodicPolicy policy({0,1,0,1,0,1,0,1,0,1});
    // periodicPolicy policy({0, 0, 0, 1, 1, 1, 0, 0, 1, 1});

    belief startB = belief({prob(1), prob(0), prob(0)});

    constexpr std::size_t reps = 1000000;
    // auto res = quantifyThreshold_s(startB, pomdp, 10, policy);
    // auto res = quantifyExpectation_s(startB,pomdp,policy, reps);
    auto [res,res_exp] = quantify_steps_until_threshold_m<periodicPolicy, lineLikePomdp>(std::move(startB), pomdp, policy, prob(0.9));
    // for (auto &&a: res) {
    // auto res = quantifyThreshold_m(std::move(startB), pomdp, 10, policy)[0];
    // auto res = quantify_steps_until_threshold_m<periodicPolicy, lineLikePomdp>(std::move(startB), pomdp, policy, prob(0.5));
    std::cout << res<< std::endl;
    std::cout << res_exp << std::endl;
    // }
    return 0;
}
