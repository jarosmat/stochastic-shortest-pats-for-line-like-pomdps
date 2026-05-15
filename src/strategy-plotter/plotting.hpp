#ifndef INC_2026_JAROS_PLOTTING_HPP
#define INC_2026_JAROS_PLOTTING_HPP
#include "periodicPolicy.hpp"
#include "lineLikePomdp.hpp"
#include "quantifier.hpp"


periodicPolicy getPolicy(const std::vector<std::size_t> &counts);

std::vector<prob> plot_probs(std::vector<prob> &&probs, const std::size_t T);

#endif //INC_2026_JAROS_PLOTTING_HPP
