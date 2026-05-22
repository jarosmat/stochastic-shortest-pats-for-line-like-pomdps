#include "lineLikePomdp.hpp"

lineLikePomdp::lineLikePomdp(std::vector<prob> &&probs) : states(probs.size() + 1), probs_(std::move(probs)) {
}

lineLikePomdp::lineLikePomdp(const std::vector<prob> &probs) : states(probs.size() + 1), probs_(probs) {
}

belief lineLikePomdp::UpdateBelief(belief &&bel, const action action) const {
    if (action >= states - 1) {
        throw std::runtime_error("Invalid action");
    }

    bel[action + 1] = bel[action + 1] + (probs_[action] * bel[action]);
    bel[action] = bel[action] * (prob(1) - probs_[action]);

    return bel;
}

belief lineLikePomdp::UpdateBelief(const belief &bel, const action action) const {
    belief res = bel;
    return UpdateBelief(std::move(res), action);
}

belief lineLikePomdp::SimulateAction(const action action, const state state) const {
    if (state >= states) {
        throw std::runtime_error("Invalid state");
    }

    if (action >= states - 1) {
        throw std::runtime_error("Invalid action");
    }

    auto res = belief(states);


    if (state == states - 1) {
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

bool lineLikePomdp::IsFinalState(const state state) const {
    return state == states - 1;
}

std::vector<prob> lineLikePomdp::GetFinalStatesProbs(belief &&bel) const {
    return {bel[states - 1]};
}

double lineLikePomdp::GetExpectedCost(const belief &belief, action a) const {
    return 1.0 - static_cast<double>(belief[states - 1]);
}

bool lineLikePomdp::EndBelief(const belief &bel) const {
    return bel[states - 1] == prob(1.0);
}

bool lineLikePomdp::IsAtEndProb(const belief &bel, const prob prob) const {
    return bel[states - 1] >= prob;
}

prob lineLikePomdp::ProbAtEnd(const belief &bel) const {
    return bel[states - 1];
}
