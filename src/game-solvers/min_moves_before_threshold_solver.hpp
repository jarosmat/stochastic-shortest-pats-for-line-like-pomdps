#ifndef INC_2026_JAROS_MIN_MOVES_BEFORE_THRESHOLD_SOLVER_HPP
#define INC_2026_JAROS_MIN_MOVES_BEFORE_THRESHOLD_SOLVER_HPP
#include "lineLikePomdp.hpp"
#include "periodicPolicy.hpp"


class min_moves_before_threshold_solver {
public:
    static std::pair<periodic_policy, std::pair<std::size_t, double>> solve(const line_like_pomdp& pomdp, prob threshold, const belief& start_b);
};


#endif //INC_2026_JAROS_MIN_MOVES_BEFORE_THRESHOLD_SOLVER_HPP
