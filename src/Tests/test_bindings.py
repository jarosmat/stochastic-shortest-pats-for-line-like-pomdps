import pypomdp


def test_prob():
    p0 = pypomdp.prob()
    p1 = pypomdp.prob(0.5)
    p2 = pypomdp.prob(0.3)

    assert abs(p0.value()) < 1e-9, "constructor failed"

    assert abs(p1.value() - 0.5) < 1e-9, "prob.value() failed"
    assert abs(float(p1) - 0.5) < 1e-9, "prob float conversion failed"

    p_add = p1 + p2
    assert abs(p_add.value() - 0.8) < 1e-9, "prob addition failed"

    p_sub = p1 - p2
    assert abs(p_sub.value() - 0.2) < 1e-9, "prob subtraction failed"

    p_mul = p1 * p2
    assert abs(p_mul.value() - 0.15) < 1e-9, "prob multiplication failed"

    p_div = p1 / p2
    assert abs(p_div.value() - 0.5 / 0.3) < 1e-9, "prob division failed"

    p_compound = pypomdp.prob(1.0)
    p_compound += p1
    assert abs(p_compound.value() - 1.5) < 1e-9, "prob += failed"

    assert p1 != p2, "prob != failed"
    assert p1 > p2, "prob > failed"
    assert p2 < p1, "prob < failed"
    assert p1 >= p2, "prob >= failed"
    assert p2 <= p1, "prob <= failed"

    print("prob class: All tests passed!")


def test_belief():
    b1 = pypomdp.belief(5)
    assert len(b1) == 5, "belief constructor with size failed"

    b2 = pypomdp.belief([pypomdp.prob(0.1), pypomdp.prob(0.2), pypomdp.prob(0.7)])
    assert len(b2) == 3, "belief constructor with vector failed"

    b1[0] = pypomdp.prob(1.0)
    b1[1] = pypomdp.prob(0.0)
    b1[2] = pypomdp.prob(0.0)
    b1[3] = pypomdp.prob(0.0)
    b1[4] = pypomdp.prob(0.0)

    assert abs(b1[0].value() - 1.0) < 1e-9, "belief getitem failed"
    assert abs(b1[1].value() - 0.0) < 1e-9, "belief getitem failed"

    print("belief class: All tests passed!")


def test_line_like_pomdp():
    probs = [pypomdp.prob(0.1), pypomdp.prob(0.2), pypomdp.prob(0.3), pypomdp.prob(0.4)]
    llp = pypomdp.LineLikePomdp(probs)

    bel = pypomdp.belief(5)
    bel[0] = pypomdp.prob(1.0)
    bel[1] = pypomdp.prob(0.0)
    bel[2] = pypomdp.prob(0.0)
    bel[3] = pypomdp.prob(0.0)
    bel[4] = pypomdp.prob(0.0)

    updated = llp.update_belief(bel, 0)
    assert abs(updated[0].value() - 0.9) < 1e-9, "update_belief failed for state 0"
    assert abs(updated[1].value() - 0.1) < 1e-9, "update_belief failed for state 1"

    sim = llp.simulate_action(0, 0)
    assert abs(sim[0].value() - 0.9) < 1e-9, "simulate_action failed"
    assert abs(sim[1].value() - 0.1) < 1e-9, "simulate_action failed"

    assert llp.is_final_state(4) == True, "is_final_state failed for final state"
    assert llp.is_final_state(0) == False, "is_final_state failed for non-final state"

    prob_at_end = llp.prob_at_end(bel)
    assert abs(prob_at_end.value()) < 1e-9, "prob_at_end failed"

    assert llp.end_belief(bel) == False, "end_belief failed"

    threshold = pypomdp.prob(0.5)
    assert llp.is_at_end_prob(bel, threshold) == False, "is_at_end_prob failed"

    cost = llp.get_expected_cost(bel, 0)
    assert abs(cost - 1.0) < 1e-9, "get_expected_cost failed"

    final_probs = llp.get_final_states_probs(bel)
    assert len(final_probs) == 1, "get_final_states_probs returned wrong size"
    assert abs(final_probs[0].value() - 0.0) < 1e-9, "get_final_states_probs failed"

    print("LineLikePomdp class: All tests passed!")


def test_integration():
    probs = [pypomdp.prob(0.5)]
    llp = pypomdp.LineLikePomdp(probs)

    bel = pypomdp.belief(2)
    bel[0] = pypomdp.prob(1.0)
    bel[1] = pypomdp.prob(0.0)

    for _ in range(3):
        bel = llp.update_belief(bel, 0)

    assert abs(bel[0].value() - 0.125) < 1e-9, "integration test failed"
    assert abs(bel[1].value() - 0.875) < 1e-9, "integration test failed"

    print("Integration: All tests passed!")


def check_policy(pol: pypomdp.ExpandedPolicy, res: list[int]):
    i = 0
    for val in pol:
        if i == len(res):
            break
        assert val == res[i]
        i += 1

    pol.reset()
    assert pol.current() == 0


def test_expanded_policy():
    policy = pypomdp.ExpandedPolicy([0, 1, 2], 3)
    res = [0, 0, 1, 1, 2, 2, 3]
    check_policy(policy, res)
    policy2 = pypomdp.ExpandedPolicy([], 4)
    res2 = [0, 1, 2, 3, 4]
    check_policy(policy2, res2)

    print("ExpandedPolicy: All tests passed!")


def test_periodic_policy():
    policy = pypomdp.PeriodicPolicy([0, 1, 2])
    res = [0, 1, 2]
    check_policy(policy, res)
    check_policy(policy, [0, 1, 2, 0, 1, 2])

    print("PeriodicPolicy: All tests passed!")


def test_solver_bindings():
    ll = pypomdp.LineLikePomdp([pypomdp.prob(0.5), pypomdp.prob(0.5)])
    bel = pypomdp.belief(3)
    bel[0] = pypomdp.prob(1.0)
    bel[1] = pypomdp.prob(0.0)
    bel[2] = pypomdp.prob(0.0)
    exp_pol, pr = pypomdp.max_prob_t_tries_solve(ll, 10, bel)
    exp_pol.reset()
    check_policy(exp_pol, [0, 0, 0, 0, 0, 1, 1, 1, 1, 1])

    threshold = pypomdp.prob(0.5)
    per_pol, metrics = pypomdp.min_moves_before_threshold_solve(ll, threshold, bel)
    moves, exp = metrics
    per_pol.reset()
    check_policy(per_pol, [0, 1, 0, 1])
    assert moves == 4
    assert exp == 3

    print("Solvers: All tests passed!")



if __name__ == "__main__":
    try:
        test_prob()
        test_belief()
        test_line_like_pomdp()
        test_integration()
        test_expanded_policy()
        test_periodic_policy()
        test_solver_bindings()

    except AssertionError as e:
        print(f"TEST FAILED: {e}")
    except ImportError as e:
        print(f"IMPORT ERROR: {e}")
        print("Make sure pypomdp module is built and in your Python path.")
