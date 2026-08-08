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

    print("  prob class: All tests passed!")


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

    print("  belief class: All tests passed!")


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

    print("  LineLikePomdp class: All tests passed!")


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

    print("  Integration: All tests passed!")


if __name__ == "__main__":
    try:
        test_prob()
        test_belief()
        test_line_like_pomdp()
        test_integration()

    except AssertionError as e:
        print(f"TEST FAILED: {e}")
    except ImportError as e:
        print(f"IMPORT ERROR: {e}")
        print("Make sure pypomdp module is built and in your Python path.")
