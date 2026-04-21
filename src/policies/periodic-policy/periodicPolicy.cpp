#include "periodicPolicy.hpp"

periodicPolicy::value_type periodicPolicy::operator*() const {
    return pol[currIndex];
}

periodicPolicy &periodicPolicy::operator++() {
    if (++currIndex >= pol.size()) {
        currIndex = 0;
    }

    return *this;
}

void periodicPolicy::operator++(int) {
    ++*this;
}

void periodicPolicy::reset() {
    currIndex = 0;
}

periodicPolicy::periodicPolicy(std::vector<std::size_t> &&vec): pol(std::move(vec)) {
}

periodicPolicy::periodicPolicy(const std::vector<std::size_t> &vec): pol(vec) {
}
