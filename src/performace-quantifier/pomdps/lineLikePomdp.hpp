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

    belief UpdateBelief(belief &&bel, action action) const;

    [[nodiscard]] belief UpdateBelief(const belief &bel, action action) const;

    [[nodiscard]] belief SimulateAction(action action, state state) const;

    [[nodiscard]] bool IsFinalState(state state) const;

    std::vector<prob> GetFinalStatesProbs(belief &&bel) const;

    [[nodiscard]] double GetExpectedCost(const belief &belief, action a) const;

    bool EndBelief(const belief &bel) const;

    bool IsAtEndProb(const belief &bel, prob prob) const;

    prob ProbAtEnd(const belief &bel) const;

    std::size_t States() const;

    bool IsAtEndProb(const belief &bel, prob prob) const;

    prob ProbAtEnd(const belief &bel) const;
};


#endif //INC_2026_JAROS_LINELIKEPOMDP_HPP
