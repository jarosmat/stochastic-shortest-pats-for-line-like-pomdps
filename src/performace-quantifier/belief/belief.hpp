#ifndef INC_2026_JAROS_BELIEF_HPP
#define INC_2026_JAROS_BELIEF_HPP
#include <vector>

#include "prob.hpp"

struct belief {
    std::vector<prob> bel;

    prob &operator[](std::size_t i);

    const prob &operator[](std::size_t i) const;

    explicit belief(std::size_t states);

    explicit belief(std::vector<prob> &&val);

    explicit belief(const std::vector<prob> &val);
};


#endif //INC_2026_JAROS_BELIEF_HPP
