#ifndef INC_2026_JAROS_MAX_PROB_T_TRIES_SOLVER_HPP
#define INC_2026_JAROS_MAX_PROB_T_TRIES_SOLVER_HPP
#include "lineLikePomdp.hpp"
#include "expandedPolicy.hpp"


class max_prob_t_tries_solver {
public:
    static std::pair<expanded_policy, prob> solve(const line_like_pomdp &pomdp, std::size_t t, const belief &start_b);
};


#endif //INC_2026_JAROS_MAX_PROB_T_TRIES_SOLVER_HPP
