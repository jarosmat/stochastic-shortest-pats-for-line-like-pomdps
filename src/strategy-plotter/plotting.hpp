#ifndef INC_2026_JAROS_PLOTTING_HPP
#define INC_2026_JAROS_PLOTTING_HPP
#include "periodicPolicy.hpp"
#include "lineLikePomdp.hpp"
#include "quantifier.hpp"


periodic_policy get_policy(const std::vector<std::size_t> &counts);

std::vector<prob> plot_probs(std::vector<prob> &&probs, std::size_t t);

std::size_t max(const std::vector<prob> &vals);

void plot_table(std::vector<prob> &&base_probs, std::vector<prob> &&table_probs, std::size_t t,
                std::ostream &stream);

#endif //INC_2026_JAROS_PLOTTING_HPP
