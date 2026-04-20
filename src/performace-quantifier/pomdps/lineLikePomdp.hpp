#ifndef INC_2026_JAROS_LINELIKEPOMDP_HPP
#define INC_2026_JAROS_LINELIKEPOMDP_HPP
#include <cstddef>
#include <vector>
#include "prob.hpp"
#include "belief.hpp"

using state = std::size_t;
using action = std::size_t;

class lineLikePomdp {
    std::size_t states = 0;

    // probs_[i] is the probability to move from state i when in that state and when action i is played
    std::vector<prob> probs_;
public:
    explicit lineLikePomdp(std::vector<prob> &&probs);

    explicit lineLikePomdp(const std::vector<prob> &probs);

    belief UpdateBelief(belief &&bel, action action);


    [[nodiscard]] belief SimulateAction(action action, state state) const;
};


#endif //INC_2026_JAROS_LINELIKEPOMDP_HPP
