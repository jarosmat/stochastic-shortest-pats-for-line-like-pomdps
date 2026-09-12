#ifndef INC_2026_JAROS_LINELIKEPOMDP_HPP
#define INC_2026_JAROS_LINELIKEPOMDP_HPP

#include <vector>
#include "prob.hpp"
#include "belief.hpp"

using state = std::size_t;
using action = std::size_t;

class line_like_pomdp {
    std::size_t states_ = 0;

    // probs_[i] is the probability to move from state i when in that state and when action i is played
    std::vector<prob> probs_;

public:
    explicit line_like_pomdp(std::vector<prob> &&probs);

    explicit line_like_pomdp(const std::vector<prob> &probs);

    belief update_belief(belief &&bel, action action) const;

    [[nodiscard]] belief update_belief(const belief &bel, action action) const;

    [[nodiscard]] belief simulate_action(action action, state state) const;

    [[nodiscard]] bool is_final_state(state state) const;

    std::vector<prob> get_final_states_probs(belief &&bel) const;

    [[nodiscard]] double get_expected_cost(const belief &belief, action a) const;

    [[nodiscard]] bool end_belief(const belief &bel) const;

    [[nodiscard]] std::size_t states() const;

    [[nodiscard]] bool is_at_end_prob(const belief &bel, prob prob) const;

    [[nodiscard]] prob prob_at_end(const belief &bel) const;
};


#endif //INC_2026_JAROS_LINELIKEPOMDP_HPP
