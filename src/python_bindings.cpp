#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <vector>

#include "prob.hpp"
#include "belief.hpp"
#include "lineLikePomdp.hpp"
#include "max_prob_t_tries_solver.hpp"
#include "min_moves_before_threshold_solver.hpp"
#include "expandedPolicy.hpp"
#include "periodicPolicy.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pypomdp, m) {
    py::class_<prob>(m, "prob")
            .def(py::init<double>(), py::arg("val") = 0.0)
            .def("value", &prob::operator double)
            .def(py::self + py::self)
            .def(py::self += py::self)
            .def(py::self - py::self)
            .def(py::self -= py::self)
            .def(py::self * py::self)
            .def(py::self *= py::self)
            .def(py::self / py::self)
            .def(py::self /= py::self)
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def(py::self < py::self)
            .def(py::self > py::self)
            .def(py::self <= py::self)
            .def(py::self >= py::self)
            .def("__repr__", [](const prob &p) {
                return "prob(" + std::to_string(static_cast<double>(p)) + ")";
            })
            .def("__str__", [](const prob &p) {
                return std::to_string(static_cast<double>(p));
            })
            .def("__float__", &prob::operator double);

    py::class_<belief>(m, "belief")
            .def(py::init<std::size_t>(), py::arg("states"))
            .def(py::init<std::vector<prob> >(), py::arg("val"))
            .def("__getitem__", [](const belief &b, const std::size_t i) -> prob {
                return b[i];
            })
            .def("__setitem__", [](belief &b, const std::size_t i, const prob &val) {
                b[i] = val;
            })
            .def("__len__", [](const belief &b) {
                return b.bel.size();
            })
            .def("__repr__", [](const belief &b) {
                std::string result = "belief([";
                for (std::size_t i = 0; i < b.bel.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(static_cast<double>(b[i]));
                }
                result += "])";
                return result;
            });
    py::class_<lineLikePomdp>(m, "LineLikePomdp")
            .def(py::init<std::vector<prob> >(), py::arg("probs"))
            .def("update_belief", [](const lineLikePomdp &self, const belief &bel, const action a) {
                return self.UpdateBelief(bel, a);
            }, py::arg("bel"), py::arg("action"))
            .def("update_belief_move", [](const lineLikePomdp &self, belief bel, const action a) {
                return self.UpdateBelief(std::move(bel), a);
            }, py::arg("bel"), py::arg("action"))
            .def("simulate_action", &lineLikePomdp::SimulateAction,
                 py::arg("action"), py::arg("state"))
            .def("is_final_state", &lineLikePomdp::IsFinalState,
                 py::arg("state"))
            .def("get_final_states_probs", [](const lineLikePomdp &self, belief bel) {
                return self.GetFinalStatesProbs(std::move(bel));
            }, py::arg("bel"))
            .def("get_expected_cost", &lineLikePomdp::GetExpectedCost,
                 py::arg("belief"), py::arg("a"))
            .def("end_belief", &lineLikePomdp::EndBelief,
                 py::arg("bel"))
            .def("is_at_end_prob", &lineLikePomdp::IsAtEndProb,
                 py::arg("bel"), py::arg("threshold"))
            .def("prob_at_end", &lineLikePomdp::ProbAtEnd,
                 py::arg("bel"));

    py::class_<expanded_policy>(m, "ExpandedPolicy")
            .def(py::init<const std::vector<std::size_t> &, std::size_t>(),
                 py::arg("policy"), py::arg("max_v"))
            .def("reset", &expanded_policy::reset)
            .def("current", [](const expanded_policy &p) { return *p; })
            .def("__iter__", [](expanded_policy &p) -> expanded_policy& { return p; })
            .def("__next__", [](expanded_policy &p) -> std::size_t {
                const auto res{*p};
                ++p;
                return res;
            });

    py::class_<periodicPolicy>(m, "PeriodicPolicy")
            .def(py::init<const std::vector<std::size_t> &>(), py::arg("policy"))
            .def("reset", &periodicPolicy::reset)
            .def("current", [](const periodicPolicy &p) { return *p; })
            .def("__iter__", [](periodicPolicy &p) -> periodicPolicy& { return p; })
            .def("__next__", [](periodicPolicy &p) -> std::size_t {
                const auto res{*p};
                ++p;
                return res;
            })
            .def("len", &periodicPolicy::len);

    m.def("max_prob_t_tries_solve",
          [](const lineLikePomdp &pomdp, const std::size_t t, const belief &bel) {
              return max_prob_t_tries_solver::solve(pomdp, t, bel);
          },
          py::arg("pomdp"), py::arg("t"), py::arg("start_b"));

    m.def("min_moves_before_threshold_solve",
          [](const lineLikePomdp &pomdp, const prob threshold, const belief &start_b) {
              return min_moves_before_threshold_solver::solve(pomdp, threshold, start_b);
          },
          py::arg("pomdp"), py::arg("threshold"), py::arg("start_b"));
}
