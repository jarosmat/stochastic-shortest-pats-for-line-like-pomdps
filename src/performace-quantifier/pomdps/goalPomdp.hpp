#ifndef INC_2026_JAROS_GOALPOMDP_HPP
#define INC_2026_JAROS_GOALPOMDP_HPP
#include <cstddef>
#include <unordered_set>
#include <vector>
#include "prob.hpp"
#include "belief.hpp"

using state = std::size_t;
using action = std::size_t;
using state_prob = std::pair<state, prob>;

class costFunction {
    const std::size_t action_c;
    const std::size_t state_c;

    // represents a matrix with row i representing the costs of action in states
    std::vector<double> costs;

public:
    costFunction(std::vector<double> &&costs, std::size_t actions, std::size_t states);

    double operator ()(std::size_t action, std::size_t state) const;
};

class transitionProb {
    std::size_t state_c;
    std::vector<prob> probs;

public:
    transitionProb(std::size_t states, std::vector<prob> &&probs);

    // represents a matrix with row i, represents probabilities to end up in states from state i, after taking action this
    // function is associated with
    prob operator ()(std::size_t target, std::size_t start) const;
};

class observationProb {
    std::size_t observation_c;
    std::size_t state_c;

    // represents a matrix with row i, represents probabilities of observing all observations upon entering state i
    // when indexed, first index is state we entered and second is observation
    std::vector<prob> probs;

public:
    observationProb(std::size_t observations, std::size_t states, std::vector<prob> &&probs);

    prob operator ()(std::size_t observation, std::size_t state) const;
};


template<typename CF, typename TF, typename OF>
class goalPomdp {
    std::size_t states;
    belief initialBel;
    std::unordered_set<std::size_t> targetStates;
    std::size_t actions;
    // vector indexed by actions
    std::vector<TF> transitionProb;
    CF c;
    std::size_t observations;
    // vector indexed by actions
    std::vector<OF> observationProb;

public:
    template<typename CFC, typename TFC, typename OFC>
    goalPomdp(belief &&initialBel, std::unordered_set<std::size_t> &&targets, const std::size_t actions,
              std::vector<TFC> &&transitionProb, CFC &&c,
              const std::size_t observations, std::vector<OFC> &&observationProb) : states(initialBel.bel.size()),
        initialBel(std::move(initialBel)),
        targetStates(std::move(targets)),
        actions(actions),
        transitionProb(
            std::forward<TFC>(transitionProb)),
        c(std::forward<CFC>(c)),
        observations(observations),
        observationProb(
            std::forward<OFC>(observationProb)) {
    }

    belief UpdateBelief(belief &&bel, action action) const {
    }

    [[nodiscard]] belief UpdateBelief(const belief &bel, action action) const {
    }

    [[nodiscard]] belief SimulateAction(action action, state state) const {
        belief res(states);
        if (IsFinalState(state)) {
            res[state] = prob(1.0);
            return res;
        }

        TF &func = transitionProb[action];

        for (std::size_t state_p = 0; state_p < states; ++state_p) {
            res[state_p] = func(state_p, state);
        }

        return res;
    }

    [[nodiscard]] bool IsFinalState(const state state) const {
        return targetStates.contains(state);
    }

    std::vector<state_prob> GetFinalStatesProbs(belief &&bel) const {
        const auto size = targetStates.size();
        std::vector<state_prob> res(size);

        std::size_t i = 0;
        for (auto &&ts: targetStates) {
            res[i] = {ts, bel[ts]};
            ++i;
        }

        return res;
    }
};


#endif //INC_2026_JAROS_GOALPOMDP_HPP
