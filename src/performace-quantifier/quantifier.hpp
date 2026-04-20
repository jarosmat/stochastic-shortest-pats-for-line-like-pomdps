#ifndef INC_2026_JAROS_QUANTIFIER_HPP
#define INC_2026_JAROS_QUANTIFIER_HPP

#include "prob.hpp"
#include "belief.hpp"
#include "lineLikePomdp.hpp"

//this function is operating under the assumption, that sum of elements of probs is 1
std::size_t sampleIndex(const std::vector<prob>& probs);

bool simulateThreshold(belief &start_b, const lineLikePomdp &pomdp, std::size_t T);

prob quantifyThreshold_s(belief &&startb, const lineLikePomdp &pomdp, std::size_t T);

#endif //INC_2026_JAROS_QUANTIFIER_HPP
