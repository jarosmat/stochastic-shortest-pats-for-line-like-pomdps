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
    std::vector<prob> probs_;

    explicit lineLikePomdp(std::vector<prob> &&probs);

    explicit lineLikePomdp(const std::vector<prob> &probs);

    belief UpdateBelief(belief &&bel, action action);


    [[nodiscard]] belief SimulateAction(action action, state state) const;
};


#endif //INC_2026_JAROS_LINELIKEPOMDP_HPP
