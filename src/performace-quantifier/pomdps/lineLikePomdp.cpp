#include "lineLikePomdp.hpp"

line_like_pomdp::line_like_pomdp(std::vector<prob> &&probs) : states_(probs.size() + 1), probs_(std::move(probs)) {
}

line_like_pomdp::line_like_pomdp(const std::vector<prob> &probs) : states_(probs.size() + 1), probs_(probs) {
}

belief line_like_pomdp::update_belief(belief &&bel, const action action) const {
    if (action >= states_ - 1) {
        throw std::runtime_error("Invalid action");
    }

    bel[action + 1] = bel[action + 1] + probs_[action] * bel[action];
    bel[action] = bel[action] * (prob(1) - probs_[action]);

    return bel;
}

belief line_like_pomdp::update_belief(const belief &bel, const action action) const {
    belief res = bel;
    return update_belief(std::move(res), action);
}

belief line_like_pomdp::simulate_action(const action action, const state state) const {
    if (state >= states_) {
        throw std::runtime_error("Invalid state");
    }

    if (action >= states_ - 1) {
        throw std::runtime_error("Invalid action");
    }

    auto res = belief(states_);


    if (state == states_ - 1) {
        res[state] = prob(1);
    }

    if (action == state) {
        res[state] = prob(1) - probs_[state];
        res[state + 1] = probs_[state];
    }
    else {
        res[state] = prob(1);
    }

    return res;
}

bool line_like_pomdp::is_final_state(const state state) const {
    return state == states_ - 1;
}

std::vector<prob> line_like_pomdp::get_final_states_probs(belief &&bel) const {
    return {bel[states_ - 1]};
}

double line_like_pomdp::get_expected_cost(const belief &belief, action) const {
    return 1.0 - static_cast<double>(belief[states_ - 1]);
}

bool line_like_pomdp::end_belief(const belief &bel) const {
    return bel[states_ - 1] == prob(1.0);
}

std::size_t line_like_pomdp::states() const {
    return states_;
}

bool line_like_pomdp::is_at_end_prob(const belief &bel, const prob prob) const {
    return bel[states_ - 1] >= prob;
}

prob line_like_pomdp::prob_at_end(const belief &bel) const {
    return bel[states_ - 1];
}
