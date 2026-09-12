#include "belief.hpp"

prob & belief::operator[](const std::size_t i) {
    return bel.at(i);
}

const prob & belief::operator[](const std::size_t i) const {
    return bel.at(i);
}

belief::belief(const std::size_t states):bel(states) {
}

belief::belief(std::vector<prob> &&val):bel(std::move(val)) {
}

belief::belief(const std::vector<prob> &val): bel(val) {
}
